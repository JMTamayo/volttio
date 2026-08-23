#include <Arduino.h>

#include "config.hpp"
#include "logging/logging.hpp"
#include "peripherals/power_switch/power_switch.hpp"
#include "peripherals/power_switch/tag.hpp"

static power_switch::PowerSwitch
    powerSwitch(config::power_switch::RELAY_PIN,
                config::power_switch::RELAY_ACTIVE_HIGH
                    ? power_switch::ActiveLevel::High
                    : power_switch::ActiveLevel::Low);

static void logStatus() {
  power_switch::PowerSwitchReading reading;
  powerSwitch.read(reading);
  LOGI(power_switch::TAG, "%s", reading.serialize().c_str());
}

void setup() {}

void loop() {
  powerSwitch.energize();
  logStatus();
  delay(30000);

  powerSwitch.deenergize();
  logStatus();
  delay(30000);
}
