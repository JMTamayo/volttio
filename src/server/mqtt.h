#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <cstdint>

#include "connection.h"
#include "fibonacci.h"
#include "flash_memory.h"

namespace server {

extern const bool DEFAULT_MQTT_MESSAGE_RETAINED;

extern const char *DEFAULT_MQTT_TOPIC_SEPARATOR;

class MqttMessage {
private:
  String _subject;
  String _payload;

  const bool _retained;

public:
  MqttMessage(const char *subject, const char *payload,
              const bool retained = DEFAULT_MQTT_MESSAGE_RETAINED);

  ~MqttMessage() = default;

  const char *getSubject() const;

  const String &getPayload() const;

  const bool getRetained() const;
};

class MqttService : public domain::IConnectionHandler {
private:
  const char *_userKey;
  const char *_passwordKey;
  const char *_domainKey;
  const char *_portKey;
  domain::FlashMemory *_flashMemory;

  const char *_projectName;
  const char *_clientId;
  const char *_topicSeparator;

  PubSubClient &_client;

  const char *getUserKey() const;

  const char *getPasswordKey() const;

  const char *getDomainKey() const;

  const char *getPortKey() const;

  domain::FlashMemory *getFlashMemory() const;

  const char *getProjectName() const;

  const char *getClientId() const;

  const char *getTopicSeparator() const;

  const String getBaseTopic() const;

  const String getTopic(const char *subject) const;

  PubSubClient &getClient();

public:
  MqttService(PubSubClient &client, const char *projectName,
              const char *clientId,
              const char *userKey = domain::FLASH_KEY_MQTT_USER,
              const char *passwordKey = domain::FLASH_KEY_MQTT_PASSWORD,
              const char *domainKey = domain::FLASH_KEY_MQTT_DOMAIN,
              const char *portKey = domain::FLASH_KEY_MQTT_PORT);

  ~MqttService() override;

  void updateUser(const char *user);

  void updatePassword(const char *password);

  void updateDomain(const char *domain);

  void updatePort(uint16_t port);

  bool credentialsStored();

  bool publish(MqttMessage *message);

  void loop();

  bool
  connect(uint8_t maxRetries = domain::DEFAULT_CONNECTION_MAX_RETRIES) override;

  void disconnect() override;

  bool connected() override;
};

} // namespace server
