#include <Arduino.h>
#include <freertos/task.h>

#include "config.h"
#include "secrets.h"

#include "clock.h"
#include "stats.h"

#include "led.h"

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

TaskHandle_t syncTimeTaskHandle;
TaskHandle_t deviceStatsTaskHandle;
TaskHandle_t serverTaskHandle;

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
      if (wifi->connect())
        serverStatus = server::ServerStatus::SERVER_STATUS_CONNECT_TO_MQTT;

      break;

    case server::ServerStatus::SERVER_STATUS_CONNECT_TO_MQTT:
      if (!wifi->connected()) {
        serverStatus = server::ServerStatus::SERVER_STATUS_CONNECT_TO_WIFI;

      } else if (!mqtt->connect()) {
        serverStatus = server::ServerStatus::SERVER_STATUS_CONNECT_TO_MQTT;

      } else {
        serverStatus = server::ServerStatus::SERVER_STATUS_AVAILABLE;
      }

      break;

    case server::ServerStatus::SERVER_STATUS_AVAILABLE:
      if (!mqtt->connected()) {
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

void setup() {
  Serial.begin(115200);

  sysClock = new board::Clock(NTP_SERVER, LOCAL_TIMEZONE_OFFSET_SEC,
                              LOCAL_DAYLIGHT_OFFSET_SEC);

  stats = new board::Stats();

  builtinLed = new peripherals::Led(BUILTIN_LED_PIN);
  builtinLed->LightUp(!false);

  mqttClient = new PubSubClient(espClient);
  mqttClient->setBufferSize(MQTT_MAX_PAYLOAD_SIZE);

  wifi = new server::WifiService(WIFI_SSID, WIFI_PASSWORD);
  mqtt =
      new server::MqttService(*mqttClient, PROJECT_NAME, DEVICE_ID, MQTT_USER,
                              MQTT_PASSWORD, MQTT_BROKER, MQTT_PORT);

  serverStatus = server::ServerStatus::SERVER_STATUS_CONNECT_TO_WIFI;

  mqttMessageQueue =
      xQueueCreate(MQTT_MESSAGE_QUEUE_SIZE, sizeof(server::MqttMessage *));

  xTaskCreate(syncTimeTask, SYNC_TIME_TASK_NAME, SYNC_TIME_TASK_STACK_SIZE,
              nullptr, SYNC_TIME_TASK_PRIORITY, &syncTimeTaskHandle);

  xTaskCreate(deviceStatsTask, DEVICE_STATS_TASK_NAME,
              DEVICE_STATS_TASK_STACK_SIZE, nullptr, DEVICE_STATS_TASK_PRIORITY,
              &deviceStatsTaskHandle);

  xTaskCreate(serverTask, SERVER_TASK_NAME, SERVER_TASK_STACK_SIZE, nullptr,
              SERVER_TASK_PRIORITY, &serverTaskHandle);
}

void loop() {}