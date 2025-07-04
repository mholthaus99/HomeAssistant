#include "HADevice.h"

const char* toComponent(DeviceType type) {
     switch (type) {
     case DeviceType::Switch: return "switch";
     case DeviceType::Light: return "light";
     case DeviceType::Sensor: return "sensor";
     case DeviceType::Lock: return "lock";
     case DeviceType::BinarySensor: return "binary_sensor";
     default: return "switch";
     }
}

String HADeviceConnection::sanitizeName(const String& name) {
     String result = name;
     result.replace(' ', '_');
     return result;
}
void HADeviceConnection::generateDiscoveryPayload(const String& name) {
     StaticJsonDocument<512> doc;
     doc["name"] = getTopics().uiName();  // e.g. MyServo
     doc["unique_id"] = getTopics().id(); // e.g. esp_bcff4dd02fba_MyServo
     doc["command_topic"] = getTopics().command();
     doc["state_topic"] = getTopics().state();
     doc["availability_topic"] = getTopics().availability();
     doc["payload_on"] = "ON";
     doc["payload_off"] = "OFF";
     doc["retain"] = true;

     JsonObject device = doc.createNestedObject("device");
     device["identifiers"] = getTopics().id();       // e.g. esp_bcff4dd02fba_MyServo
     device["name"] = getTopics().uiName();          // e.g. MyServo
     device["manufacturer"] = "ESP8266";
     device["model"] = "Custom MQTT Device";

     serializeJson(doc, discoveryPayload);
}
 

HADeviceConnection::HADeviceConnection(MQTTConnection& mqttClient, const String& name, DeviceType type,
     IDevice& commandHandler)
     : mqttClient(mqttClient), deviceTopics(sanitizeName(name)), type(type), device(commandHandler) {
     generateDiscoveryPayload(name);
     commandHandler.setDeviceConnection(*this);
}

void HADeviceConnection::connect() {
     publishDiscovery();
     publishAvailability("online");
     subscribeToCommands();
}

void HADeviceConnection::publishDiscovery() {
     if (!mqttClient.isConnected()) {
          Serial.println("MQTT not connected at publishDiscovery.");
          return;
     }

     Serial.print("Publishing to: ");
     Serial.println(deviceTopics.discovery(toComponent(type)));

     Serial.print("Payload: ");
     Serial.println(discoveryPayload);

     // mqttClient.setBufferSize(512); // Ensure buffer size is sufficient for discovery payload

     mqttClient.publish(deviceTopics.discovery(toComponent(type)).c_str(), discoveryPayload.c_str(), true);
}

void HADeviceConnection::publishState(const String& state) {
     mqttClient.publish(deviceTopics.state().c_str(), state.c_str(), true);
}

void HADeviceConnection::publishAvailability(const String& availability) {
     mqttClient.publish(deviceTopics.availability().c_str(), availability.c_str(), true);
}

void HADeviceConnection::subscribeToCommands() {
     mqttClient.subscribe(deviceTopics.command(), [this](const String& topic, const String& payload) {
          device.processCommandCallback(payload);
          });
     Serial.print("Subscribed to command topic: ");
     Serial.println(deviceTopics.command());
}

const MqttTopicGenerator& HADeviceConnection::getTopics() const {
     return deviceTopics;
}
