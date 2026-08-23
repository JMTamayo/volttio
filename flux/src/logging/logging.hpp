#pragma once

#include <cstdint>

namespace logging {

enum class Level : std::uint8_t {
  Error = 1,
  Warn,
  Info,
  Debug,
  Verbose,
};

void log(Level level, const char *tag, const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));

} // namespace logging

#define LOGE(tag, ...)                                                         \
  ::logging::log(::logging::Level::Error, (tag), __VA_ARGS__)
#define LOGW(tag, ...)                                                         \
  ::logging::log(::logging::Level::Warn, (tag), __VA_ARGS__)
#define LOGI(tag, ...)                                                         \
  ::logging::log(::logging::Level::Info, (tag), __VA_ARGS__)
#define LOGD(tag, ...)                                                         \
  ::logging::log(::logging::Level::Debug, (tag), __VA_ARGS__)
#define LOGV(tag, ...)                                                         \
  ::logging::log(::logging::Level::Verbose, (tag), __VA_ARGS__)