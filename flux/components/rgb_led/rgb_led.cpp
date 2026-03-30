#include <esp_log.h>
#include <led_strip_types.h>

#include "rgb_led.h"

namespace peripherals {

const char *TAG = "rgb_led";

namespace {

struct rgb_color_values_t {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

esp_err_t get_rgb_color_values(rgb_color color, rgb_color_values_t *values) {
  if (values == nullptr) {
    ESP_LOGE(TAG, "RGB led get color values failed: values is nullptr");
    return ESP_ERR_INVALID_ARG;
  }

  switch (color) {
  case rgb_color::OFF:
    values->red = 0;
    values->green = 0;
    values->blue = 0;
    break;

  case rgb_color::RED:
    values->red = 255;
    values->green = 0;
    values->blue = 0;
    break;

  case rgb_color::YELLOW:
    values->red = 255;
    values->green = 255;
    values->blue = 0;
    break;

  case rgb_color::GREEN:
    values->red = 0;
    values->green = 255;
    values->blue = 0;
    break;

  case rgb_color::BLUE:
    values->red = 0;
    values->green = 0;
    values->blue = 255;
    break;

  default:
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

} // namespace

RGBLed::RGBLed(gpio_num_t pin) : pin_(pin), strip_(nullptr) {}

RGBLed::~RGBLed(void) {
  if (strip_ != nullptr) {
    ESP_LOGI(TAG, "Releasing RGB led (gpio=%d)", pin_);
    this->light(rgb_color::OFF);
    led_strip_del(strip_);
    strip_ = nullptr;
  }
}

esp_err_t RGBLed::begin(void) {
  if (strip_ != nullptr) {
    ESP_LOGW(TAG, "RGB led already initialized");
    return ESP_OK;
  }

  ESP_LOGI(TAG, "Initializing RGB led (gpio=%d)", pin_);

  led_strip_config_t strip_config = {
      .strip_gpio_num = pin_,
      .max_leds = 1,
      .led_model = LED_MODEL_WS2812,
      .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_RGB,
      .flags =
          {
              .invert_out = false,
          },
  };

  led_strip_rmt_config_t rmt_config = {
      .clk_src = static_cast<rmt_clock_source_t>(0),
      .resolution_hz = 0,
      .mem_block_symbols = 0,
      .flags =
          {
              .with_dma = false,
          },
  };

  esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &strip_);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "RGB led initialization failed: %s (0x%x)",
             esp_err_to_name(err), err);
    return err;
  }

  err = led_strip_clear(strip_);
  if (err != ESP_OK) {
    led_strip_del(strip_);
    strip_ = nullptr;
    ESP_LOGE(TAG, "RGB led clear failed: %s (0x%x)", esp_err_to_name(err), err);
    return err;
  }

  err = this->light(rgb_color::OFF);
  if (err != ESP_OK) {
    led_strip_del(strip_);
    strip_ = nullptr;
    ESP_LOGE(TAG, "RGB led light off failed: %s (0x%x)", esp_err_to_name(err),
             err);
    return err;
  }

  ESP_LOGI(TAG, "RGB led initialized");

  return ESP_OK;
}

esp_err_t RGBLed::light(rgb_color color) {
  if (strip_ == nullptr) {
    ESP_LOGE(TAG, "RGB led not initialized");
    return ESP_ERR_INVALID_STATE;
  }

  rgb_color_values_t values;
  esp_err_t err = get_rgb_color_values(color, &values);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "RGB led get color values failed: %s (0x%x)",
             esp_err_to_name(err), err);
    return err;
  }

  err = led_strip_set_pixel(strip_, 0, values.red, values.green, values.blue);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "RGB led set pixel failed: %s (0x%x)", esp_err_to_name(err),
             err);
    return err;
  }

  err = led_strip_refresh(strip_);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "RGB led refresh failed: %s (0x%x)", esp_err_to_name(err),
             err);
    return err;
  }

  return ESP_OK;
}

} // namespace peripherals
