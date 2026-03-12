#pragma once

#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <cstdint>

#include "connection.h"
#include "fibonacci.h"
#include "flash_memory.h"

namespace server {

extern const bool DEFAULT_MQTT_MESSAGE_RETAINED;

extern const uint8_t DEFAULT_MQTT_MESSAGE_QOS;

extern const char *DEFAULT_MQTT_TOPIC_SEPARATOR;

class MqttMessage {
private:
  String _subject;
  String _payload;

  const uint8_t _qos;
  const bool _retained;

public:
  MqttMessage(const char *subject, const char *payload,
              const bool retained = DEFAULT_MQTT_MESSAGE_RETAINED,
              const uint8_t qos = DEFAULT_MQTT_MESSAGE_QOS);

  ~MqttMessage() = default;

  const char *getSubject() const;

  const String &getPayload() const;

  const uint8_t getQos() const;

  const bool getRetained() const;
};

class MqttService : public domain::IConnectionHandler {
private:
  const char *_userKey;
  const char *_passwordKey;
  const char *_domainKey;
  const char *_portKey;
  domain::FlashReader *_flashReader;

  const char *_projectName;
  const char *_clientId;
  const char *_topicSeparator;

  AsyncMqttClient &_client;

  const char *getUserKey() const;

  const char *getPasswordKey() const;

  const char *getDomainKey() const;

  const char *getPortKey() const;

  domain::FlashReader *getFlashReader() const;

  const char *getProjectName() const;

  const char *getClientId() const;

  const char *getTopicSeparator() const;

  const String getBaseTopic() const;

  const String getTopic(const char *subject) const;

  AsyncMqttClient &getClient();

public:
  MqttService(AsyncMqttClient &client, const char *projectName,
              const char *clientId,
              const char *userKey = domain::FLASH_KEY_MQTT_USER,
              const char *passwordKey = domain::FLASH_KEY_MQTT_PASSWORD,
              const char *domainKey = domain::FLASH_KEY_MQTT_DOMAIN,
              const char *portKey = domain::FLASH_KEY_MQTT_PORT);

  ~MqttService() override;

  bool credentialsStored();

  bool publish(MqttMessage *message);

  bool
  connect(uint8_t maxRetries = domain::DEFAULT_CONNECTION_MAX_RETRIES) override;

  bool connected() override;
};

} // namespace server
