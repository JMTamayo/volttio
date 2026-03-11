#pragma once

#include <WiFi.h>

#include "connection.h"
#include "fibonacci.h"
#include "flash_memory.h"

namespace server {

class WifiService : public domain::IConnectionHandler {
private:
  const char *_ssidKey;
  const char *_passwordKey;
  domain::FlashMemory *_flashMemory;

  const char *getSsidKey() const;

  const char *getPasswordKey() const;

  domain::FlashMemory *getFlashMemory() const;

public:
  WifiService(const char *ssidKey = domain::FLASH_KEY_WIFI_SSID,
              const char *passwordKey = domain::FLASH_KEY_WIFI_PASSWORD);

  ~WifiService() override;

  void updateSsid(const char *ssid);

  void updatePassword(const char *password);

  bool credentialsStored();

  bool connect(const uint8_t maxRetries =
                   domain::DEFAULT_CONNECTION_MAX_RETRIES) override;

  void disconnect() override;

  bool connected() override;
};

} // namespace server
