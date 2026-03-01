#include "mqtt.h"

namespace server {

const bool DEFAULT_MQTT_MESSAGE_RETAINED = false;

const char *DEFAULT_MQTT_TOPIC_SEPARATOR = "/";

MqttMessage::MqttMessage(const char *subject, const char *payload,
                         const bool retained)
    : _subject(subject != nullptr ? subject : ""),
      _payload(payload != nullptr ? payload : ""), _retained(retained) {}

const char *MqttMessage::getSubject() const { return _subject.c_str(); }

const String &MqttMessage::getPayload() const { return _payload; }

const bool MqttMessage::getRetained() const { return _retained; }

const char *MqttService::getUser() const { return _user; }

const char *MqttService::getPassword() const { return _password; }

const char *MqttService::getDomain() const { return _domain; }

const uint16_t MqttService::getPort() const { return _port; }

const char *MqttService::getProjectName() const { return _projectName; }

const char *MqttService::getClientId() const { return _clientId; }

const char *MqttService::getTopicSeparator() const { return _topicSeparator; }

const String MqttService::getBaseTopic() const {
  return String(this->getTopicSeparator()) + String(this->getProjectName()) +
         String(this->getTopicSeparator()) + String(this->getClientId()) +
         String(this->getTopicSeparator());
}

const String MqttService::getTopic(const char *subject) const {
  return String(this->getBaseTopic()) + String(subject);
}

PubSubClient &MqttService::getClient() { return _client; }

MqttService::MqttService(PubSubClient &client, const char *projectName,
                         const char *clientId, const char *user,
                         const char *password, const char *domain,
                         const uint16_t port)
    : _user(user), _password(password), _domain(domain), _port(port),
      _projectName(projectName), _clientId(clientId),
      _topicSeparator(DEFAULT_MQTT_TOPIC_SEPARATOR), _client(client) {}

bool MqttService::publish(MqttMessage *message) {
  if (message == nullptr)
    return false;

  String topic = this->getTopic(message->getSubject());

  bool ok = this->getClient().publish(
      topic.c_str(), message->getPayload().c_str(), message->getRetained());

  return ok;
}

void MqttService::loop() { this->getClient().loop(); }

bool MqttService::connect(const uint8_t maxRetries) {
  this->getClient().setServer(this->getDomain(), this->getPort());
  this->getClient().connect(this->getClientId(), this->getUser(),
                            this->getPassword());

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

void MqttService::disconnect() { this->getClient().disconnect(); }

bool MqttService::connected() { return this->getClient().connected(); }

} // namespace server