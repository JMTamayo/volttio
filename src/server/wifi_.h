#pragma once

#include <WiFi.h>

#include "fibonacci.h"
#include "server.h"

namespace server {

class WifiService : public server::IConnectionHandler {
private:
  const char *_ssid;
  const char *_pass;

  const char *getSsid() const;

  const char *getPass() const;

public:
  WifiService(const char *ssid, const char *pass);

  ~WifiService() = default;

  bool connect(const uint8_t maxRetries =
                   server::DEFAULT_CONNECTION_MAX_RETRIES) override;

  void disconnect() override;

  bool connected() override;
};

} // namespace server