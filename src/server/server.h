#pragma once

namespace server {

const uint8_t DEFAULT_CONNECTION_MAX_RETRIES = 6;

class IConnectionHandler {

public:
  virtual ~IConnectionHandler() = default;

  virtual bool
  connect(const uint8_t maxRetries = DEFAULT_CONNECTION_MAX_RETRIES) = 0;

  virtual void disconnect() = 0;

  virtual bool connected() = 0;
};

enum class ServerStatus : uint8_t {
  SERVER_STATUS_CONNECT_TO_WIFI = 0,
  SERVER_STATUS_CONNECT_TO_MQTT = 1,
  SERVER_STATUS_AVAILABLE = 2,
};

} // namespace server