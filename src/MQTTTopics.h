#pragma once
#include <Arduino.h>
#include "DeviceInfo.h"

class MqttTopicGenerator {
public:
     String macId;           // raw esp_bcff4dd02fba
     String objectId;        // e.g. "MyServo"
     String deviceGroupId;   // e.g. "esp_bcff4dd02fba_MyServo"

     MqttTopicGenerator(const String& objectId)
          : macId(DeviceInfo::getDeviceId()), objectId(objectId),
          deviceGroupId(macId + "_" + objectId) {
     }

     String command() const {
          return macId + "/" + objectId + "/command";
     }

     String state() const {
          return macId + "/" + objectId + "/state";
     }

     String availability() const {
          return macId + "/status";
     }

     String discovery(const String& domain) const {
          return "homeassistant/" + domain + "/" + deviceGroupId + "/config";
     }

     const String& id() const {
          return deviceGroupId;
     }

     const String& uiName() const {
          return objectId;
     }
};
     