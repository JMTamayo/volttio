#include "peripherals/power_switch/application/power_switch_service.hpp"

#include "logging/logging.hpp"
#include "peripherals/power_switch/tag.hpp"

namespace power_switch {

PowerSwitchService::PowerSwitchService(IPowerSwitch &powerSwitch)
    : _powerSwitch(powerSwitch), _energized(false) {}

error::Error PowerSwitchService::energize() {
  LOGI(TAG, "Energizing power switch");

  error::Error err = _powerSwitch.close();
  if (err != error::Error::Ok) {
    LOGE(TAG, "Failed to energize power switch: %s", err.toString());
    return err;
  }

  _energized = true;
  LOGI(TAG, "Power switch energized");
  return err;
}

error::Error PowerSwitchService::deenergize() {
  LOGI(TAG, "Deenergizing power switch");

  error::Error err = _powerSwitch.open();
  if (err != error::Error::Ok) {
    LOGE(TAG, "Failed to deenergize power switch: %s", err.toString());
    return err;
  }

  _energized = false;
  LOGI(TAG, "Power switch deenergized");
  return err;
}

error::Error PowerSwitchService::read(PowerSwitchReading &reading) const {
  reading.data = PowerSwitchState{.energized = _energized};
  reading.error = error::Error::Ok;

  LOGD(TAG, "Power switch is currently %s",
       _energized ? "energized" : "deenergized");
  return reading.error;
}

} // namespace power_switch
