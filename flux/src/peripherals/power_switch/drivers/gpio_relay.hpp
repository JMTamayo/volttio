#pragma once

#include <cstdint>

#include "error/error.hpp"
#include "peripherals/power_switch/domain/i_power_switch.hpp"

namespace power_switch {

enum class ActiveLevel : uint8_t {
  High,
  Low,
};

class GpioRelay : public IPowerSwitch {
public:
  GpioRelay(uint8_t pin, ActiveLevel activeLevel);

  GpioRelay(const GpioRelay &) = delete;
  GpioRelay &operator=(const GpioRelay &) = delete;

  error::Error close() override;

  error::Error open() override;

private:
  uint8_t activeLevelValue() const;
  uint8_t inactiveLevelValue() const;

  uint8_t _pin;
  ActiveLevel _activeLevel;
};

} // namespace power_switch
