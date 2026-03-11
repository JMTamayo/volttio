#include <Arduino.h>
#include <freertos/task.h>

#include "config.h"
#include "secrets.h"

#include "clock.h"
#include "stats.h"

#include "led.h"

#include "ble.h"

#include "mqtt.h"
#include "server.h"
#include "wifi_.h"

WiFiClient espClient;
PubSubClient *mqttClient;

board::Clock *sysClock;
board::Stats *stats;

peripherals::Led *builtinLed;

server::WifiService *wifi;
server::MqttService *mqtt;
server::ServerStatus serverStatus;

ble::BleService *bleService;

TaskHandle_t syncTimeTaskHandle;
TaskHandle_t deviceStatsTaskHandle;
TaskHandle_t serverTaskHandle;
TaskHandle_t bleTaskHandle;

QueueHandle_t mqttMessageQueue;

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

void deviceStatsTask(void *pvParameters) {
  String datetime;

  for (;;) {
    if (serverStatus == server::ServerStatus::SERVER_STATUS_AVAILABLE &&
        sysClock->getCurrentTimeISO8601(&datetime)) {
      String jsonStats = stats->getStats(datetime);

      server::MqttMessage *message =
          new server::MqttMessage(MQTT_SUBJECT_DEVICE_STATS, jsonStats.c_str(),
                                  MQTT_SUBJECT_DEVICE_STATS_RETAINED);
      if (xQueueSend(mqttMessageQueue, &message,
                     pdMS_TO_TICKS(MQTT_MESSAGE_QUEUE_DELAY_MS)) != pdTRUE) {
        delete message;
      }

      vTaskDelay(pdMS_TO_TICKS(DEVICE_STATS_TASK_DELAY_MS));

    } else {
      vTaskDelay(pdMS_TO_TICKS(DEVICE_STATS_TASK_RETRY_DELAY_MS));
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
        serverStatus = server::ServerStatus::SERVER_STATUS_AVAILABLE;
      }

      break;

    case server::ServerStatus::SERVER_STATUS_AVAILABLE:
      if (!mqtt->connected()) {
        Serial.println("[Server] MQTT lost, reconnecting");
        serverStatus = server::ServerStatus::SERVER_STATUS_CONNECT_TO_MQTT;
        builtinLed->LightUp(!false);

      } else {
        builtinLed->LightUp(!true);

        mqtt->loop();

        server::MqttMessage *message;
        while (xQueueReceive(mqttMessageQueue, &message, 0) == pdPASS) {
          mqtt->publish(message);
          delete message;
        }
      }

      break;
    }

    vTaskDelay(pdMS_TO_TICKS(SERVER_TASK_DELAY_MS));
  }
}

void bleTask(void *pvParameters) {
  for (;;) {
    if (!bleService->started())
      bleService->start();

    vTaskDelay(pdMS_TO_TICKS(BLE_TASK_DELAY_MS));
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("[Setup] Initializing volttio");

  sysClock = new board::Clock(NTP_SERVER, LOCAL_TIMEZONE_OFFSET_SEC,
                              LOCAL_DAYLIGHT_OFFSET_SEC);

  stats = new board::Stats();

  builtinLed = new peripherals::Led(BUILTIN_LED_PIN);
  builtinLed->LightUp(!false);

  mqttClient = new PubSubClient(espClient);
  mqttClient->setBufferSize(MQTT_MAX_PAYLOAD_SIZE);

  wifi = new server::WifiService();
  mqtt = new server::MqttService(*mqttClient, PROJECT_NAME, DEVICE_ID);

  serverStatus = server::ServerStatus::SERVER_STATUS_CONNECT_TO_WIFI;

  bleService = new ble::BleService(PROJECT_NAME, DEVICE_ID);

  mqttMessageQueue =
      xQueueCreate(MQTT_MESSAGE_QUEUE_SIZE, sizeof(server::MqttMessage *));

  xTaskCreate(syncTimeTask, "syncTimeTask", SYNC_TIME_TASK_STACK_SIZE, nullptr,
              SYNC_TIME_TASK_PRIORITY, &syncTimeTaskHandle);

  xTaskCreate(deviceStatsTask, "deviceStatsTask", DEVICE_STATS_TASK_STACK_SIZE,
              nullptr, DEVICE_STATS_TASK_PRIORITY, &deviceStatsTaskHandle);

  xTaskCreate(serverTask, "serverTask", SERVER_TASK_STACK_SIZE, nullptr,
              SERVER_TASK_PRIORITY, &serverTaskHandle);

  xTaskCreate(bleTask, "bleTask", BLE_TASK_STACK_SIZE, nullptr,
              BLE_TASK_PRIORITY, &bleTaskHandle);

  Serial.println("[Setup] All tasks created");
}

void loop() {}
