#pragma once
#include "interfaces/HADevice.h"
#include "interfaces/IDevice.h"
#include "Arduino.h"

class HADeviceConnectionWithAdjustmentControl : public HADeviceConnection {
public:
          HADeviceConnectionWithAdjustmentControl(MQTTConnection& mqttClient, const String& name, IDevice& handler)
                    : HADeviceConnection(mqttClient, name, DeviceType::Switch, handler) {
          }

          void publishExtraButtons() {
                    publishButton("Up", "+1");
                    publishButton("Down", "-1");
                    publishButton("Up 10", "+10");
                    publishButton("Down 10", "-10");
                    publishButton("Set On", "SET_ON");
                    publishButton("Set Off", "SET_OFF");
          }

private:
          void publishButton(const String& name, const String& payload) {
                    String buttonName = sanitizeName(name);
                    String uniqueButtonName = getTopics().id() + "_" + buttonName; // e.g. esp_bcff4dd02fba_MyServo_up10
                    String topic = "homeassistant/button/" + uniqueButtonName + "/config";

                    StaticJsonDocument<384> doc;
                    doc["name"] = name; // Friendly name, e.g. "Up 10"
                    doc["unique_id"] = uniqueButtonName;
                    doc["command_topic"] = getTopics().command();
                    doc["payload_press"] = payload;

                    JsonObject device = doc.createNestedObject("device");
                    device["identifiers"] = getTopics().id();       // e.g. esp_bcff4dd02fba_MyServo
                    device["name"] = getTopics().uiName();          // e.g. MyServo
                    device["manufacturer"] = "ESP8266";
                    device["model"] = "Custom MQTT Device";

                    Serial.println("Publishing button config to: " + topic);

                    char buffer[384];
                    serializeJson(doc, buffer);
                    mqttClient.publish(topic.c_str(), buffer, true);
          }
      

          static String sanitizeName(const String& name) {
                    String result = name;
                    result.replace(' ', '_');
                    return result;
          }
};
