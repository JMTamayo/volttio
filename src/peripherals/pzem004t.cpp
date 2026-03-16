#include "pzem004t.h"

#include <Arduino.h>
#include <ArduinoJson.h>

namespace peripherals {

const uint8_t Pzem004t::getRxPin() const { return _rxPin; }

const uint8_t Pzem004t::getTxPin() const { return _txPin; }

Pzem004t::Pzem004t(const uint8_t rxPin, const uint8_t txPin)
    : _rxPin(rxPin), _txPin(txPin), _pzem(Serial2, rxPin, txPin) {}

String Pzem004t::Read() {
  const float voltage = _pzem.voltage();
  const float current = _pzem.current();
  const float power = _pzem.power();
  const float energyKwh = _pzem.energy();
  const float frequency = _pzem.frequency();
  const float powerFactor = _pzem.pf();

  JsonDocument doc;
  JsonObject data = doc["data"].to<JsonObject>();

  if (std::isnan(voltage))
    data["voltage"] = nullptr;
  else
    data["voltage"] = roundf(voltage * 10.0f) / 10.0f;

  if (std::isnan(current))
    data["current"] = nullptr;
  else
    data["current"] = roundf(current * 1000.0f) / 1000.0f;

  if (std::isnan(power))
    data["power"] = nullptr;
  else
    data["power"] = roundf(power * 10.0f) / 10.0f;

  if (std::isnan(energyKwh))
    data["active_energy"] = nullptr;
  else
    data["active_energy"] = roundf(energyKwh * 1000.0f * 10.0f) / 10.0f;

  if (std::isnan(frequency))
    data["frequency"] = nullptr;
  else
    data["frequency"] = roundf(frequency * 10.0f) / 10.0f;

  if (std::isnan(powerFactor))
    data["power_factor"] = nullptr;
  else {
    const float pf =
        (powerFactor > 1.0f) ? 1.0f : (powerFactor < 0.0f ? 0.0f : powerFactor);
    data["power_factor"] = roundf(pf * 100.0f) / 100.0f;
  }

  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

} // namespace peripherals
