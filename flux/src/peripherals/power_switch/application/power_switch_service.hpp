#pragma once

#include "error/error.hpp"

#include "peripherals/power_switch/domain/i_power_switch.hpp"
#include "peripherals/power_switch/domain/power_switch_reading.hpp"
#include "peripherals/sensors/domain/i_readable.hpp"

namespace power_switch {

class PowerSwitchService : public sensors::IReadable<PowerSwitchState> {
public:
  PowerSwitchService(IPowerSwitch &powerSwitch);

  error::Error energize();

  error::Error deenergize();

  error::Error read(PowerSwitchReading &reading) const override;

private:
  IPowerSwitch &_powerSwitch;
  bool _energized;
};

} // namespace power_switch
