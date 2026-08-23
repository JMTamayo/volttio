#include "peripherals/power_switch/power_switch.hpp"

namespace power_switch {

PowerSwitch::PowerSwitch(uint8_t pin, ActiveLevel activeLevel)
    : PowerSwitchService(*new GpioRelay(pin, activeLevel)) {}

} // namespace power_switch
