#include "mqtt.h"

namespace server {

const bool DEFAULT_MQTT_MESSAGE_RETAINED = false;

const uint8_t DEFAULT_MQTT_MESSAGE_QOS = 0;

const char *DEFAULT_MQTT_TOPIC_SEPARATOR = "/";

MqttMessage::MqttMessage(const char *subject, const char *payload,
                         const bool retained, const uint8_t qos)
    : _subject(subject != nullptr ? subject : ""),
      _payload(payload != nullptr ? payload : ""), _qos(qos),
      _retained(retained) {}

const char *MqttMessage::getSubject() const { return _subject.c_str(); }

const String &MqttMessage::getPayload() const { return _payload; }

const uint8_t MqttMessage::getQos() const { return _qos; }

const bool MqttMessage::getRetained() const { return _retained; }

const char *MqttService::getUserKey() const { return _userKey; }

const char *MqttService::getPasswordKey() const { return _passwordKey; }

const char *MqttService::getDomainKey() const { return _domainKey; }

const char *MqttService::getPortKey() const { return _portKey; }

domain::FlashReader *MqttService::getFlashReader() const {
  return _flashReader;
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

AsyncMqttClient &MqttService::getClient() { return _client; }

MqttService::MqttService(AsyncMqttClient &client, const char *projectName,
                         const char *clientId, const char *userKey,
                         const char *passwordKey, const char *domainKey,
                         const char *portKey)
    : _userKey(userKey), _passwordKey(passwordKey), _domainKey(domainKey),
      _portKey(portKey),
      _flashReader(new domain::FlashReader(domain::FLASH_NAMESPACE_MQTT)),
      _projectName(projectName), _clientId(clientId),
      _topicSeparator(DEFAULT_MQTT_TOPIC_SEPARATOR), _client(client) {}

MqttService::~MqttService() { delete _flashReader; }

bool MqttService::credentialsStored() {
  String user = this->getFlashReader()->readString(this->getUserKey());
  String domain = this->getFlashReader()->readString(this->getDomainKey());
  uint16_t port = this->getFlashReader()->readUint16(this->getPortKey());
  return !user.isEmpty() && !domain.isEmpty() && port > 0;
}

bool MqttService::publish(MqttMessage *message) {
  if (message == nullptr)
    return false;

  String topic = this->getTopic(message->getSubject());
  const String &payload = message->getPayload();
  const char *payloadPtr = payload.isEmpty() ? nullptr : payload.c_str();
  size_t payloadLen = payload.length();

  uint16_t id =
      this->getClient().publish(topic.c_str(), message->getQos(),
                                message->getRetained(), payloadPtr, payloadLen);

  return id != 0;
}

bool MqttService::connect(const uint8_t maxRetries) {
  String user = this->getFlashReader()->readString(this->getUserKey());
  String password = this->getFlashReader()->readString(this->getPasswordKey());
  String domain = this->getFlashReader()->readString(this->getDomainKey());
  uint16_t port = this->getFlashReader()->readUint16(this->getPortKey());

  Serial.printf("[MQTT] Connecting to %s:%u as %s\n", domain.c_str(), port,
                user.c_str());

  this->getClient().setClientId(this->getClientId());
  this->getClient().setServer(domain.c_str(), port);
  this->getClient().setCredentials(user.c_str(), password.c_str());
  this->getClient().connect();

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

bool MqttService::connected() { return this->getClient().connected(); }

} // namespace server
