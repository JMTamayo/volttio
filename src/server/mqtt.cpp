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

const char *MqttService::getUserKey() const { return _userKey; }

const char *MqttService::getPasswordKey() const { return _passwordKey; }

const char *MqttService::getDomainKey() const { return _domainKey; }

const char *MqttService::getPortKey() const { return _portKey; }

domain::FlashMemory *MqttService::getFlashMemory() const {
  return _flashMemory;
}

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
                         const char *clientId, const char *userKey,
                         const char *passwordKey, const char *domainKey,
                         const char *portKey)
    : _userKey(userKey), _passwordKey(passwordKey), _domainKey(domainKey),
      _portKey(portKey),
      _flashMemory(new domain::FlashMemory(domain::FLASH_NAMESPACE_MQTT)),
      _projectName(projectName), _clientId(clientId),
      _topicSeparator(DEFAULT_MQTT_TOPIC_SEPARATOR), _client(client) {}

MqttService::~MqttService() { delete _flashMemory; }

void MqttService::updateUser(const char *user) {
  this->getFlashMemory()->saveString(this->getUserKey(), user);
}

void MqttService::updatePassword(const char *password) {
  this->getFlashMemory()->saveString(this->getPasswordKey(), password);
}

void MqttService::updateDomain(const char *domain) {
  this->getFlashMemory()->saveString(this->getDomainKey(), domain);
}

void MqttService::updatePort(uint16_t port) {
  this->getFlashMemory()->saveUint16(this->getPortKey(), port);
}

bool MqttService::credentialsStored() {
  String user = this->getFlashMemory()->readString(this->getUserKey());
  String domain = this->getFlashMemory()->readString(this->getDomainKey());
  uint16_t port = this->getFlashMemory()->readUint16(this->getPortKey());
  return !user.isEmpty() && !domain.isEmpty() && port > 0;
}

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
  String user = this->getFlashMemory()->readString(this->getUserKey());
  String password = this->getFlashMemory()->readString(this->getPasswordKey());
  String domain = this->getFlashMemory()->readString(this->getDomainKey());
  uint16_t port = this->getFlashMemory()->readUint16(this->getPortKey());

  Serial.printf("[MQTT] Connecting to %s:%u as %s\n", domain.c_str(), port,
                user.c_str());

  this->getClient().setServer(domain.c_str(), port);
  this->getClient().connect(this->getClientId(), user.c_str(),
                            password.c_str());

  uint8_t retries = 0;

  while (!this->connected()) {
    if (retries > maxRetries) {
      Serial.println("[MQTT] Connection failed, max retries reached");
      return false;
    }

    uint32_t delayMs = domain::Fibonacci::get(retries) * 1000;
    vTaskDelay(pdMS_TO_TICKS(delayMs));

    retries++;
  }

  Serial.printf("[MQTT] Connected, client: %s\n", this->getClientId());
  return true;
}

void MqttService::disconnect() { this->getClient().disconnect(); }

bool MqttService::connected() { return this->getClient().connected(); }

} // namespace server
