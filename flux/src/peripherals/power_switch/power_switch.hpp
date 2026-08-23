#pragma once

#include "peripherals/power_switch/application/power_switch_service.hpp"
#include "peripherals/power_switch/drivers/gpio_relay.hpp"

namespace power_switch {

class PowerSwitch : public PowerSwitchService {
public:
  PowerSwitch(uint8_t pin, ActiveLevel activeLevel);
};

} // namespace power_switch
