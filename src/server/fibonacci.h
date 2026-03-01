#pragma once

#include <cstdint>

namespace server {

class Fibonacci {
public:
  static uint32_t get(const uint32_t n);
};

} // namespace server