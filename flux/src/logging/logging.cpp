#include "logging/logging.hpp"

#include <Arduino.h>

#include <cstdarg>
#include <cstdio>

#define LOG_BUFFER_SIZE 256

namespace logging {

void log(Level level, const char *tag, const char *fmt, ...) {
  char message[LOG_BUFFER_SIZE];

  va_list args;
  va_start(args, fmt);
  vsnprintf(message, sizeof(message), fmt, args);
  va_end(args);

  switch (level) {
  case Level::Error:
    ESP_LOGE(tag, "%s", message);
    break;
  case Level::Warn:
    ESP_LOGW(tag, "%s", message);
    break;
  case Level::Info:
    ESP_LOGI(tag, "%s", message);
    break;
  case Level::Debug:
    ESP_LOGD(tag, "%s", message);
    break;
  case Level::Verbose:
    ESP_LOGV(tag, "%s", message);
    break;
  }
}

} // namespace logging
