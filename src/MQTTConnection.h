#pragma once
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <functional>
#include <map>

class MQTTConnection {
public:
          using MessageCallback = std::function<void(const String& topic, const String& payload)>;

          MQTTConnection(const char* wifiSsid, const char* wifiPassword,
                    const char* mqttServer, int mqttPort,
                    const char* mqttUser, const char* mqttPass);

          void begin();
          void loop();
          bool isConnected();

          void subscribe(const String& topic, MessageCallback cb);
          bool publish(const String& topic, const String& payload, bool retain = false);

          PubSubClient& getClient();

private:
          WiFiClient wifiClient;
          PubSubClient mqttClient;
          const char* ssid;
          const char* password;
          const char* mqttServer;
          const char* mqttUser;
          const char* mqttPass;
          int mqttPort;

          unsigned long lastReconnectAttempt = 0;
          std::map<String, MessageCallback> topicCallbacks;

          void connectWiFi();
          void connectMQTT();
          void handleMessage(char* topic, byte* payload, unsigned int length);
};
