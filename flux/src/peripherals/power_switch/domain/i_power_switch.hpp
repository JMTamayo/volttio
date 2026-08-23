#pragma once

#include "error/error.hpp"

namespace power_switch {

class IPowerSwitch {
public:
  virtual ~IPowerSwitch() = default;

  virtual error::Error close() = 0;

  virtual error::Error open() = 0;
};

} // namespace power_switch
