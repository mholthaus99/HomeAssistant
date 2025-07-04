#include "MQTTConnection.h"
#include <Arduino.h>

MQTTConnection::MQTTConnection(const char* wifiSsid, const char* wifiPassword,
          const char* mqttServer, int mqttPort,
          const char* mqttUser, const char* mqttPass)
          : ssid(wifiSsid),
          password(wifiPassword),
          mqttServer(mqttServer),
          mqttPort(mqttPort),
          mqttUser(mqttUser),
          mqttPass(mqttPass),
          mqttClient(wifiClient)
{
          // Set the MQTT callback to the class handler
          mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
                    this->handleMessage(topic, payload, length);
                    });
}

void MQTTConnection::begin() {
          connectWiFi();
          mqttClient.setServer(mqttServer, mqttPort);
          connectMQTT();
}

void MQTTConnection::connectWiFi() {
          if (WiFi.status() == WL_CONNECTED) return;

          Serial.print("Connecting to WiFi: ");
          Serial.println(ssid);
          WiFi.begin(ssid, password);

          while (WiFi.status() != WL_CONNECTED) {
                    delay(500);
                    Serial.print(".");
          }
          Serial.println("\nWiFi connected.");
}

void MQTTConnection::connectMQTT() {
          if (mqttClient.connected()) return;

          unsigned long now = millis();
          if (now - lastReconnectAttempt < 5000) return;  // reconnect interval throttle

          lastReconnectAttempt = now;

          String clientId = "esp_" + String(ESP.getChipId());
          Serial.print("Connecting to MQTT as ");
          Serial.println(clientId);

          if (mqttClient.connect(clientId.c_str(), mqttUser, mqttPass)) {
                    Serial.println("MQTT connected.");
                    // Resubscribe all topics after reconnect
                    for (const auto& entry : topicCallbacks) {
                              mqttClient.subscribe(entry.first.c_str());
                              Serial.print("Subscribed to ");
                              Serial.println(entry.first);
                    }
          }
          else {
                    Serial.print("MQTT connect failed, rc=");
                    Serial.println(mqttClient.state());
          }
}

void MQTTConnection::loop() {
          if (!mqttClient.connected()) {
                    connectWiFi();
                    connectMQTT();
          }
          mqttClient.loop();
}

bool MQTTConnection::isConnected() {
          return mqttClient.connected();
}

void MQTTConnection::subscribe(const String& topic, MessageCallback cb) {
          // Save callback
          topicCallbacks[topic] = cb;

          if (mqttClient.connected()) {
                    mqttClient.subscribe(topic.c_str());
                    Serial.print("Subscribed to topic: ");
                    Serial.println(topic);
          }
}

bool MQTTConnection::publish(const String& topic, const String& payload, bool retain) {
          if (!mqttClient.connected()) {
                    Serial.println("MQTT not connected, cannot publish.");
                    return false;
          }
          return mqttClient.publish(topic.c_str(), payload.c_str(), retain);
}

PubSubClient& MQTTConnection::getClient() {
          return mqttClient;
}

void MQTTConnection::handleMessage(char* topic, byte* payload, unsigned int length) {
          String topicStr(topic);
          String payloadStr;
          payloadStr.reserve(length);

          for (unsigned int i = 0; i < length; i++) {
                    payloadStr += (char)payload[i];
          }

          auto it = topicCallbacks.find(topicStr);
          if (it != topicCallbacks.end()) {
                    // Call the registered callback
                    it->second(topicStr, payloadStr); //Second refers the value in the map, which is the callback functio
          }
          else {
                    Serial.print("No callback registered for topic: ");
                    Serial.println(topicStr);
          }
}
