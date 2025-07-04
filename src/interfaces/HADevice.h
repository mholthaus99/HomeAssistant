#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "MQTTConnection.h"
#include <functional>
#include "MQTTTopics.h"
#include "IDevice.h" // Ensure this path is correct and the file exists

enum class DeviceType {
          Switch,
          Light,
          Sensor,
          Lock,
          BinarySensor
};

const char* toComponent(DeviceType type);

class HADeviceConnection {
protected:
          MQTTConnection& mqttClient;

private:
          MqttTopicGenerator deviceTopics;
          String discoveryPayload;
          DeviceType type;
          IDevice& device;

          static String sanitizeName(const String& name);
          void generateDiscoveryPayload(const String& name);

public:
          HADeviceConnection(MQTTConnection& mqttClient, const String& name, DeviceType type, IDevice& commandHandler);

          void connect();
          void publishDiscovery();
          void publishState(const String& state);
          void publishAvailability(const String& availability);
          void subscribeToCommands();
          const MqttTopicGenerator& getTopics() const;
};
