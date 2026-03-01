#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <cstdint>

#include "fibonacci.h"
#include "server.h"

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

class MqttService : public server::IConnectionHandler {
private:
  const char *_user;
  const char *_password;
  const char *_domain;
  const uint16_t _port;

  const char *_projectName;
  const char *_clientId;
  const char *_topicSeparator;

  PubSubClient &_client;

  const char *getUser() const;

  const char *getPassword() const;

  const char *getDomain() const;

  const uint16_t getPort() const;

  const char *getProjectName() const;

  const char *getClientId() const;

  const char *getTopicSeparator() const;

  const String getBaseTopic() const;

  const String getTopic(const char *subject) const;

  PubSubClient &getClient();

public:
  MqttService(PubSubClient &client, const char *projectName,
              const char *clientId, const char *user, const char *password,
              const char *domain, const uint16_t port);

  ~MqttService() = default;

  bool publish(MqttMessage *message);

  void loop();

  bool
  connect(uint8_t maxRetries = server::DEFAULT_CONNECTION_MAX_RETRIES) override;

  void disconnect() override;

  bool connected() override;
};

} // namespace server