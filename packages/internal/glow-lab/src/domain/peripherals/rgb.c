#include "domain/peripherals/rgb.h"
#include "config.h"

#include <esp_log.h>
#include <led_strip.h>

static const char *TAG = "rgb";

static led_strip_handle_t led_strip;

esp_err_t rgb_begin(void)
{
    if (led_strip != NULL)
    {
        ESP_LOGW(TAG, "rgb_begin called but strip is already initialized");
        return ESP_OK;
    }

    ESP_LOGI(
        TAG,
        "Initializing strip (gpio=%d, max_leds=%u, rmt_resolution_hz=%lu)",
        RGB_STRIP_GPIO,
        (unsigned)RGB_STRIP_MAX_LEDS,
        (unsigned long)RGB_STRIP_RMT_RESOLUTION_HZ);

    led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_STRIP_GPIO,
        .max_leds = RGB_STRIP_MAX_LEDS,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_RGB,
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = RGB_STRIP_RMT_RESOLUTION_HZ,
        .flags.with_dma = false,
    };

    esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "led_strip_new_rmt_device failed: %s (0x%x)", esp_err_to_name(err), err);
        return err;
    }

    err = led_strip_clear(led_strip);
    if (err != ESP_OK)
    {
        led_strip_del(led_strip);
        led_strip = NULL;
        ESP_LOGE(TAG, "led_strip_clear failed: %s (0x%x)", esp_err_to_name(err), err);
        return err;
    }

    ESP_LOGI(TAG, "RGB strip initialized");
    return ESP_OK;
}

esp_err_t rgb_set_color_value(rgb_color_value_t value)
{
    if (led_strip == NULL)
    {
        ESP_LOGE(TAG, "rgb_set_color_value called before rgb_begin");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = led_strip_set_pixel(led_strip, 0, value.red, value.green, value.blue);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "led_strip_set_pixel failed: %s (0x%x)", esp_err_to_name(err), err);
        return err;
    }

    err = led_strip_refresh(led_strip);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "led_strip_refresh failed: %s (0x%x)", esp_err_to_name(err), err);
        return err;
    }

    return ESP_OK;
}

esp_err_t rgb_set_color(rgb_color_t color)
{
    rgb_color_value_t value;

    switch (color)
    {
    case RGB_COLOR_OFF:
        value = (rgb_color_value_t){.red = 0, .green = 0, .blue = 0};
        break;

    case RGB_COLOR_RED:
        value = (rgb_color_value_t){.red = 255, .green = 0, .blue = 0};
        break;

    case RGB_COLOR_YELLOW:
        value = (rgb_color_value_t){.red = 255, .green = 255, .blue = 0};
        break;

    case RGB_COLOR_GREEN:
        value = (rgb_color_value_t){.red = 0, .green = 255, .blue = 0};
        break;

    case RGB_COLOR_BLUE:
        value = (rgb_color_value_t){.red = 0, .green = 0, .blue = 255};
        break;

    default:
        ESP_LOGE(TAG, "Invalid color: %d", (int)color);
        return ESP_ERR_INVALID_STATE;
    }

    return rgb_set_color_value(value);
}
