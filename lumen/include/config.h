#pragma once

#include "driver/gpio.h"

#ifndef RGB_STRIP_GPIO
#define RGB_STRIP_GPIO GPIO_NUM_8
#endif

#ifndef RGB_STRIP_MAX_LEDS
#define RGB_STRIP_MAX_LEDS 1U
#endif

#ifndef RGB_STRIP_RMT_RESOLUTION_HZ
#define RGB_STRIP_RMT_RESOLUTION_HZ 10e6
#endif
