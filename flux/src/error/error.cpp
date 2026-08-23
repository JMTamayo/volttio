#include "error/error.hpp"

namespace error {

const char *Error::toString() const {
  switch (_value) {
  case Ok:
    return "OK";
  case NotFound:
    return "NOT_FOUND";
  case InvalidArg:
    return "INVALID_ARG";
  case InvalidState:
    return "INVALID_STATE";
  case NotInitialized:
    return "NOT_INITIALIZED";
  case NoMemory:
    return "NO_MEMORY";
  case IoError:
    return "IO_ERROR";
  case Unknown:
    return "UNKNOWN";
  }

  return "UNKNOWN";
}

} // namespace error
