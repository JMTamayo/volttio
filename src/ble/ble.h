#pragma once

#include <BLECharacteristic.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include "flash_memory.h"

namespace ble {

class BleConnectionCallbacks : public BLEServerCallbacks {
public:
  void onDisconnect(BLEServer *pServer) override;
};

class BleStringCallbacks : public BLECharacteristicCallbacks {
private:
  domain::FlashMemory *_flashMemory;
  const char *_key;

  domain::FlashMemory *getFlashMemory() const;

  const char *getKey() const;

public:
  BleStringCallbacks(domain::FlashMemory *flashMemory, const char *key);

  void onWrite(BLECharacteristic *pCharacteristic) override;
};

class BleUint16Callbacks : public BLECharacteristicCallbacks {
private:
  domain::FlashMemory *_flashMemory;
  const char *_key;

  domain::FlashMemory *getFlashMemory() const;

  const char *getKey() const;

public:
  BleUint16Callbacks(domain::FlashMemory *flashMemory, const char *key);

  void onWrite(BLECharacteristic *pCharacteristic) override;
};

class BleService {
private:
  const char *_deviceName;
  const char *_serviceUuid;

  domain::FlashMemory *_wifiFlash;
  domain::FlashMemory *_mqttFlash;

  BLEServer *_server;

  const char *getDeviceName() const;

  const char *getServiceUuid() const;

  domain::FlashMemory *getWifiFlash() const;

  domain::FlashMemory *getMqttFlash() const;

  BLEServer *getServer() const;

  void setServer(BLEServer *server);

public:
  BleService(const char *deviceName, const char *serviceUuid);

  ~BleService();

  bool start();

  void stop();

  bool started();

  bool clientConnected();
};

} // namespace ble
