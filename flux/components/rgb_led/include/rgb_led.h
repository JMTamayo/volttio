#pragma once

#include <cstdint>
#include <driver/gpio.h>
#include <esp_err.h>
#include <led_strip.h>

namespace peripherals {

/**
Colors supported by the `RGBLed` abstraction.

The current implementation exposes a small set of status-oriented colors
through the `rgb_color` enum class. Each color uses binary channel
intensities (`0` or `255`), without intermediate brightness control.
*/
enum class rgb_color : uint8_t {
  OFF = 0,
  RED,
  YELLOW,
  GREEN,
  BLUE,
};

/**
Controls a single addressable RGB LED connected to a GPIO.

This class wraps ESP-IDF's `led_strip` driver and exposes a small,
application-focused API for using the LED as a status indicator. The rest
of the codebase does not need to know about RMT details or the LED protocol.

The implementation is intentionally narrow: it is tuned for one
WS2812-compatible LED connected directly to the GPIO passed to the
constructor. It is not meant to be a generic abstraction for long LED
strips or for devices with different timing or color ordering.

Default assumptions used by `begin()`:
- Only one LED is managed (`max_leds = 1`).
- Timing is configured for `LED_MODEL_WS2812`.
- The LED is assumed to use `RGB` component order.
- The output signal is not inverted (`invert_out = false`).
- RMT clock source, resolution, and symbol memory are delegated to the
  driver defaults (`clk_src = 0`, `resolution_hz = 0`,
  `mem_block_symbols = 0`).
- DMA is disabled (`with_dma = false`).
- The LED is explicitly left in `rgb_color::OFF` after initialization.

Practical implications of those defaults:
- If a strip is connected, only the first pixel is updated.
- A non-WS2812 device may not respond correctly.
- If the physical LED uses `GRB`, `BRG`, or another channel order, colors
  will appear swapped.
- If the hardware path inverts the signal, initialization must be adjusted.
- Driver-selected RMT defaults keep setup simple, but provide less explicit
  control over timing and memory usage.
- Disabling DMA is a good fit for one LED, but may be less efficient for
  longer strips or heavier refresh patterns.

The predefined colors in `light()` use full channel intensity (`255`),
which favors visibility but increases instantaneous current draw and does
not provide dimming.
*/
class RGBLed {
private:
  const gpio_num_t pin_;
  led_strip_handle_t strip_;

public:
  /**
  Creates an instance bound to the GPIO that carries the LED data signal.
  */
  RGBLed(gpio_num_t pin);
  RGBLed(const RGBLed &) = delete;
  RGBLed &operator=(const RGBLed &) = delete;

  /**
  Releases the LED driver.

  If the LED was initialized, the implementation tries to turn it off
  before destroying the handle so the indicator is left in a safe state.
  */
  ~RGBLed(void);

  /**
  Initializes the LED driver and leaves it turned off.

  Returns `ESP_OK` on success. Otherwise, returns the ESP-IDF error code
  reported by the underlying driver setup or reset operations.
  */
  esp_err_t begin(void);

  /**
  Updates the LED using one of the values defined by `rgb_color`.

  Returns `ESP_OK` when the color is applied successfully,
  `ESP_ERR_INVALID_STATE` if the LED was not initialized, or
  `ESP_ERR_INVALID_ARG` if the color is not supported.
  */
  esp_err_t light(rgb_color color);
};

} // namespace peripherals
