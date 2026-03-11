#include "wifi_.h"

#include <cstdint>

namespace server {

const char *WifiService::getSsidKey() const { return _ssidKey; }

const char *WifiService::getPasswordKey() const { return _passwordKey; }

domain::FlashMemory *WifiService::getFlashMemory() const {
  return _flashMemory;
}

WifiService::WifiService(const char *ssidKey, const char *passwordKey)
    : _ssidKey(ssidKey), _passwordKey(passwordKey),
      _flashMemory(new domain::FlashMemory(domain::FLASH_NAMESPACE_WIFI)) {}

WifiService::~WifiService() { delete _flashMemory; }

void WifiService::updateSsid(const char *ssid) {
  this->getFlashMemory()->saveString(this->getSsidKey(), ssid);
}

void WifiService::updatePassword(const char *password) {
  this->getFlashMemory()->saveString(this->getPasswordKey(), password);
}

bool WifiService::credentialsStored() {
  String ssid = this->getFlashMemory()->readString(this->getSsidKey());
  return !ssid.isEmpty();
}

bool WifiService::connect(const uint8_t maxRetries) {
  String ssid = this->getFlashMemory()->readString(this->getSsidKey());
  String pass = this->getFlashMemory()->readString(this->getPasswordKey());

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  uint8_t retries = 0;

  while (!this->connected()) {
    if (retries > maxRetries) {
      Serial.println("[WiFi] Connection failed, max retries reached");
      return false;
    }

    uint32_t delayMs = domain::Fibonacci::get(retries) * 1000;
    vTaskDelay(pdMS_TO_TICKS(delayMs));

    retries++;
  }

  Serial.printf("[WiFi] Connected, IP: %s\n",
                WiFi.localIP().toString().c_str());
  return true;
}

void WifiService::disconnect() { WiFi.disconnect(); }

bool WifiService::connected() { return WiFi.status() == WL_CONNECTED; }

} // namespace server
