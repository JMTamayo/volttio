#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>

#include "domain/peripherals/rgb.h"

static const char *TAG = "main";

static esp_err_t build()
{
    esp_err_t err = rgb_begin();
    if (err != ESP_OK)
        return err;

    err = rgb_set_color(RGB_COLOR_OFF);
    if (err != ESP_OK)
        return err;

    ESP_LOGI(TAG, "RGB successfully initialized");

    return ESP_OK;
}

void app_main(void)
{
    ESP_ERROR_CHECK(build());
    ESP_LOGI(TAG, "Build successfully completed");

    while (true)
    {
        rgb_set_color(RGB_COLOR_OFF);
        vTaskDelay(pdMS_TO_TICKS(1000));

        rgb_set_color(RGB_COLOR_RED);
        vTaskDelay(pdMS_TO_TICKS(1000));

        rgb_set_color(RGB_COLOR_YELLOW);
        vTaskDelay(pdMS_TO_TICKS(1000));

        rgb_set_color(RGB_COLOR_GREEN);
        vTaskDelay(pdMS_TO_TICKS(1000));

        rgb_set_color(RGB_COLOR_BLUE);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
