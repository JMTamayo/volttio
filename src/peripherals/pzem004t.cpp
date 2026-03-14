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
  if (std::isnan(voltage))
    return String();

  const float current = _pzem.current();
  const float power = _pzem.power();
  const float energyKwh = _pzem.energy();
  const float frequency = _pzem.frequency();
  const float powerFactor = _pzem.pf();

  if (std::isnan(current) || std::isnan(power) || std::isnan(energyKwh) ||
      std::isnan(frequency) || std::isnan(powerFactor))
    return String();

  const float powerFactorClamped =
      (powerFactor > 1.0f) ? 1.0f : (powerFactor < 0.0f ? 0.0f : powerFactor);
  const float activeEnergyWh = energyKwh * 1000.0f;

  JsonDocument doc;
  JsonObject data = doc["data"].to<JsonObject>();
  data["voltage"] = roundf(voltage * 10.0f) / 10.0f;
  data["current"] = roundf(current * 1000.0f) / 1000.0f;
  data["power"] = roundf(power * 10.0f) / 10.0f;
  data["active_energy"] = roundf(activeEnergyWh * 10.0f) / 10.0f;
  data["frequency"] = roundf(frequency * 10.0f) / 10.0f;
  data["power_factor"] = roundf(powerFactorClamped * 100.0f) / 100.0f;

  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

} // namespace peripherals
