#include "wifi_.h"

#include <cstdint>

namespace server {

const char *WifiService::getSsid() const { return _ssid; }

const char *WifiService::getPass() const { return _pass; }

WifiService::WifiService(const char *ssid, const char *pass)
    : _ssid(ssid), _pass(pass) {}

bool WifiService::connect(const uint8_t maxRetries) {
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  WiFi.begin(this->getSsid(), this->getPass());

  uint8_t retries = 0;

  while (!this->connected()) {
    if (retries > maxRetries)
      return false;

    uint32_t delayMs = server::Fibonacci::get(retries) * 100;
    vTaskDelay(pdMS_TO_TICKS(delayMs));

    retries++;
  }

  return true;
}

void WifiService::disconnect() { WiFi.disconnect(); }

bool WifiService::connected() { return WiFi.status() == WL_CONNECTED; }

} // namespace server