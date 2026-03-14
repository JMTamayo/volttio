#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <cstring>

#include "config.h"
#include "secrets.h"

#include "clock.h"
#include "stats.h"

#include "led.h"
#include "pzem004t.h"

#include "ble.h"

#include "mqtt.h"
#include "server.h"
#include "wifi_.h"

AsyncMqttClient mqttClient;

board::Clock *sysClock;
board::Stats *stats;

peripherals::Led *builtinLed;
peripherals::Pzem004t *pzem;

server::WifiService *wifi;
server::MqttService *mqtt;
server::ServerStatus serverStatus;

ble::BleService *bleService;

TaskHandle_t syncTimeTaskHandle;
TaskHandle_t serverTaskHandle;

TaskHandle_t deviceStatsTaskHandle;
TaskHandle_t controlTaskHandle;

QueueHandle_t MqttPublishingEventQueue;
QueueHandle_t MqttSubscriptionsEventQueue;

void syncTimeTask(void *pvParameters) {
  for (;;) {
    if (serverStatus == server::ServerStatus::SERVER_STATUS_AVAILABLE) {
      sysClock->syncTime();
      vTaskDelay(pdMS_TO_TICKS(SYNC_TIME_TASK_DELAY_MS));

    } else {
      vTaskDelay(pdMS_TO_TICKS(SYNC_TIME_TASK_RETRY_DELAY_MS));
    }
  }
}

void serverTask(void *pvParameters) {
  for (;;) {
    switch (serverStatus) {
    case server::ServerStatus::SERVER_STATUS_CONNECT_TO_WIFI:
      if (!wifi->credentialsStored())
        break;

      if (wifi->connect())
        serverStatus = server::ServerStatus::SERVER_STATUS_CONNECT_TO_MQTT;

      break;

    case server::ServerStatus::SERVER_STATUS_CONNECT_TO_MQTT:
      if (!wifi->connected()) {
        Serial.println("[Server] WiFi lost, reconnecting");
        serverStatus = server::ServerStatus::SERVER_STATUS_CONNECT_TO_WIFI;

      } else if (!mqtt->credentialsStored()) {
        break;

      } else if (!mqtt->connect()) {
        serverStatus = server::ServerStatus::SERVER_STATUS_CONNECT_TO_MQTT;

      } else {
        Serial.println("[Server] Server available");

        mqtt->subscribe(MQTT_COMMAND_RESTART, 0);
        mqtt->subscribe(MQTT_COMMAND_PING, 0);

        serverStatus = server::ServerStatus::SERVER_STATUS_AVAILABLE;
      }

      break;

    case server::ServerStatus::SERVER_STATUS_AVAILABLE:
      if (!mqtt->connected()) {
        Serial.println("[Server] MQTT lost, reconnecting");
        serverStatus = server::ServerStatus::SERVER_STATUS_CONNECT_TO_MQTT;
        builtinLed->LightUp(false);

      } else {
        builtinLed->LightUp(true);

        server::MqttMessage *message;
        while (xQueueReceive(MqttPublishingEventQueue, &message, 0) == pdPASS) {
          mqtt->publish(message);
          delete message;
        }
      }

      break;
    }

    vTaskDelay(pdMS_TO_TICKS(SERVER_TASK_DELAY_MS));
  }
}

void deviceStatsTask(void *pvParameters) {
  String datetime;

  for (;;) {
    if (serverStatus == server::ServerStatus::SERVER_STATUS_AVAILABLE &&
        sysClock->getCurrentTimeISO8601(&datetime)) {
      String jsonStats = stats->getStats(datetime);

      server::MqttMessage *message =
          new server::MqttMessage(MQTT_SUBJECT_DEVICE_STATS, jsonStats.c_str(),
                                  MQTT_SUBJECT_DEVICE_STATS_RETAINED);
      if (xQueueSend(MqttPublishingEventQueue, &message,
                     pdMS_TO_TICKS(MQTT_PUBLISHING_EVENT_QUEUE_DELAY_MS)) !=
          pdTRUE) {
        delete message;
      }

      vTaskDelay(pdMS_TO_TICKS(DEVICE_STATS_TASK_DELAY_MS));

    } else {
      vTaskDelay(pdMS_TO_TICKS(DEVICE_STATS_TASK_RETRY_DELAY_MS));
    }
  }
}

void restartDevice() { ESP.restart(); }

void ping() {
  server::MqttMessage *message = new server::MqttMessage(
      MQTT_SUBJECT_PING, nullptr, MQTT_SUBJECT_PING_RETAINED);
  if (xQueueSend(MqttPublishingEventQueue, &message,
                 pdMS_TO_TICKS(MQTT_PUBLISHING_EVENT_QUEUE_DELAY_MS)) != pdTRUE)
    delete message;
}

void commandNotSupported(const char *command) {
  String payload = String("Command not supported: ") + command;
  server::MqttMessage *message = new server::MqttMessage(
      MQTT_SUBJECT_ERROR, payload.c_str(), MQTT_SUBJECT_ERROR_RETAINED);
  if (xQueueSend(MqttPublishingEventQueue, &message,
                 pdMS_TO_TICKS(MQTT_PUBLISHING_EVENT_QUEUE_DELAY_MS)) != pdTRUE)
    delete message;
}

void controlTask(void *pvParameters) {
  static uint32_t lastSampleTime = 0;

  for (;;) {
    server::MqttMessage *subMessage;
    if (xQueueReceive(MqttSubscriptionsEventQueue, &subMessage,
                      pdMS_TO_TICKS(MQTT_SUBSCRIPTIONS_QUEUE_DELAY_MS)) ==
        pdPASS) {
      if (strcmp(subMessage->getSubject(), MQTT_COMMAND_RESTART) == 0) {
        restartDevice();

      } else if (strcmp(subMessage->getSubject(), MQTT_COMMAND_PING) == 0) {
        ping();

      } else {
        commandNotSupported(subMessage->getSubject());
      }

      delete subMessage;
    }

    if (millis() - lastSampleTime >= DEFAULT_SAMPLING_INTERVAL_MILLISECONDS) {
      lastSampleTime = millis();
      const String jsonEnergy = pzem->Read();

      if (!jsonEnergy.isEmpty()) {
        Serial.println(jsonEnergy);
        server::MqttMessage *pubMessage = new server::MqttMessage(
            MQTT_SUBJECT_ENERGY_STATS, jsonEnergy.c_str(),
            MQTT_SUBJECT_ENERGY_STATS_RETAINED);
        if (xQueueSend(MqttPublishingEventQueue, &pubMessage,
                       pdMS_TO_TICKS(MQTT_PUBLISHING_EVENT_QUEUE_DELAY_MS)) !=
            pdTRUE)
          delete pubMessage;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(CONTROL_TASK_DELAY_MS));
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("[Setup] Initializing volttio");

  MqttPublishingEventQueue = xQueueCreate(MQTT_PUBLISHING_EVENT_QUEUE_SIZE,
                                          sizeof(server::MqttMessage *));
  MqttSubscriptionsEventQueue = xQueueCreate(MQTT_SUBSCRIPTIONS_QUEUE_SIZE,
                                             sizeof(server::MqttMessage *));

  sysClock = new board::Clock(NTP_SERVER, LOCAL_TIMEZONE_OFFSET_SEC,
                              LOCAL_DAYLIGHT_OFFSET_SEC);

  stats = new board::Stats();

  builtinLed = new peripherals::Led(BUILTIN_LED_PIN);
  builtinLed->LightUp(false);

  pzem = new peripherals::Pzem004t(PZEM_UART_RX_PIN, PZEM_UART_TX_PIN);

  wifi = new server::WifiService();

  mqtt = new server::MqttService(mqttClient, PROJECT_NAME, DEVICE_ID,
                                 MqttSubscriptionsEventQueue);

  serverStatus = server::ServerStatus::SERVER_STATUS_CONNECT_TO_WIFI;

  bleService = new ble::BleService(PROJECT_NAME, DEVICE_ID);
  bleService->start();

  xTaskCreatePinnedToCore(
      syncTimeTask, "syncTimeTask", SYNC_TIME_TASK_STACK_SIZE, nullptr,
      SYNC_TIME_TASK_PRIORITY, &syncTimeTaskHandle, SYNC_TIME_TASK_CORE);

  xTaskCreatePinnedToCore(serverTask, "serverTask", SERVER_TASK_STACK_SIZE,
                          nullptr, SERVER_TASK_PRIORITY, &serverTaskHandle,
                          SERVER_TASK_CORE);

  xTaskCreatePinnedToCore(deviceStatsTask, "deviceStatsTask",
                          DEVICE_STATS_TASK_STACK_SIZE, nullptr,
                          DEVICE_STATS_TASK_PRIORITY, &deviceStatsTaskHandle,
                          DEVICE_STATS_TASK_CORE);

  xTaskCreatePinnedToCore(controlTask, "controlTask", CONTROL_TASK_STACK_SIZE,
                          nullptr, CONTROL_TASK_PRIORITY, &controlTaskHandle,
                          CONTROL_TASK_CORE);

  Serial.println("[Setup] All tasks created");
}

void loop() {}
