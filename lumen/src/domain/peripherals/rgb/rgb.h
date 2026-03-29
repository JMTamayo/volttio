#pragma once

#include <stdint.h>
#include <esp_err.h>

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_color_value_t;

typedef enum
{
    RGB_COLOR_OFF = 0,
    RGB_COLOR_RED,
    RGB_COLOR_YELLOW,
    RGB_COLOR_GREEN,
    RGB_COLOR_BLUE,
} rgb_color_t;

esp_err_t rgb_begin(void);

esp_err_t rgb_set_color_value(rgb_color_value_t value);

esp_err_t rgb_set_color(rgb_color_t color);
