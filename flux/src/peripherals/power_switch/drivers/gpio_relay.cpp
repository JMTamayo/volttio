#include "peripherals/power_switch/drivers/gpio_relay.hpp"

#include <Arduino.h>

namespace power_switch {

GpioRelay::GpioRelay(uint8_t pin, ActiveLevel activeLevel)
    : _pin(pin), _activeLevel(activeLevel) {
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, inactiveLevelValue());
}

error::Error GpioRelay::close() {
  digitalWrite(_pin, activeLevelValue());
  return error::Error::Ok;
}

error::Error GpioRelay::open() {
  digitalWrite(_pin, inactiveLevelValue());
  return error::Error::Ok;
}

uint8_t GpioRelay::activeLevelValue() const {
  return _activeLevel == ActiveLevel::High ? HIGH : LOW;
}

uint8_t GpioRelay::inactiveLevelValue() const {
  return _activeLevel == ActiveLevel::High ? LOW : HIGH;
}

} // namespace power_switch
