#pragma once

#include "error/error.hpp"
#include "peripherals/sensors/domain/sensor_reading.hpp"

namespace sensors {

template <typename T> class IReadable {
public:
  virtual ~IReadable() = default;

  virtual error::Error read(SensorReading<T> &reading) const = 0;
};

} // namespace sensors
