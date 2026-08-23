#pragma once

#include <cstdint>

namespace error {

class Error {
public:
  enum Value : std::uint8_t {
    Ok = 0,
    NotFound,
    InvalidArg,
    InvalidState,
    NotInitialized,
    NoMemory,
    IoError,
    Unknown,
  };

  constexpr Error() : _value(Ok) {}

  constexpr Error(Value value) : _value(value) {}

  constexpr bool operator==(const Error &other) const {
    return _value == other._value;
  }

  constexpr bool operator!=(const Error &other) const {
    return _value != other._value;
  }

  const char *toString() const;

private:
  Value _value;
};

} // namespace error
