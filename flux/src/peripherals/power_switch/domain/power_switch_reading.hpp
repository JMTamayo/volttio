#pragma once

#include <string>

#include <ArduinoJson.h>

#include "peripherals/sensors/domain/sensor_reading.hpp"

namespace power_switch {

struct PowerSwitchState {
  bool energized;

  std::string serialize() const {
    JsonDocument doc;
    doc["energized"] = energized;

    std::string output;
    serializeJson(doc, output);
    return output;
  }
};

using PowerSwitchReading = sensors::SensorReading<PowerSwitchState>;

} // namespace power_switch
