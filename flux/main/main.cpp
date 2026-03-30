#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "rgb_led.h"

const char *TAG = "main";

extern "C" void app_main(void) {
  peripherals::RGBLed rgb_led(GPIO_NUM_8);
  ESP_ERROR_CHECK(rgb_led.begin());
  ESP_LOGD(TAG, "RGB led initialized");

  ESP_LOGI(TAG, "Application setup completed");

  while (true) {
    rgb_led.light(peripherals::rgb_color::OFF);
    vTaskDelay(pdMS_TO_TICKS(1000));

    rgb_led.light(peripherals::rgb_color::RED);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
