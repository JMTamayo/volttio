#pragma once

#include <concepts>
#include <optional>
#include <string>

#include <ArduinoJson.h>

#include "error/error.hpp"

namespace sensors {

template <typename T>
concept Serializable = requires(const T &value) {
  { value.serialize() } -> std::convertible_to<std::string>;
};

template <Serializable T> struct SensorReading {
  std::optional<T> data;
  error::Error error;

  std::string serialize() const {
    JsonDocument doc;

    if (data.has_value()) {
      doc["data"] = data->serialize();
    } else {
      doc["data"] = nullptr;
    }
    doc["error"] = error.toString();

    std::string output;
    serializeJson(doc, output);
    return output;
  }
};

} // namespace sensors
