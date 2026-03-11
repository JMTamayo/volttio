#pragma once

#include <Preferences.h>

namespace domain {

extern const char *FLASH_NAMESPACE_WIFI;

extern const char *FLASH_NAMESPACE_MQTT;

extern const char *FLASH_KEY_WIFI_SSID;

extern const char *FLASH_KEY_WIFI_PASSWORD;

extern const char *FLASH_KEY_MQTT_USER;

extern const char *FLASH_KEY_MQTT_PASSWORD;

extern const char *FLASH_KEY_MQTT_DOMAIN;

extern const char *FLASH_KEY_MQTT_PORT;

class FlashMemory {
private:
  const char *_namespace;
  Preferences _preferences;

  const char *getNamespace() const;

  Preferences &getPreferences();

public:
  FlashMemory(const char *ns);

  ~FlashMemory() = default;

  bool saveString(const char *key, const char *value);

  const String readString(const char *key);

  bool saveUint16(const char *key, const uint16_t value);

  uint16_t readUint16(const char *key);
};

} // namespace domain
