#include "MQTTConnection.h"
#include "ServoDevice.h"
#include "HADeviceAdjustment.h"
#include "secrets.h"
#include "StepperDevice.h"

MQTTConnection mqtt(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);

ServoDevice servoDevice(D6, 90, 15);
HADeviceConnectionWithAdjustmentControl device(mqtt, "MyServo", servoDevice);  // mqtt is passed in, not PubSubClient directly

StepperDevice stepperDevice(D1, D2, D3, D4, 200, 1.8); // Example stepper device with 200 steps per revolution and 1.8 degree step angle
HADeviceConnectionWithAdjustmentControl stepperDeviceConnection(mqtt, "MyStepper", stepperDevice); // Create connection for stepper

void setup() {
  Serial.begin(115200);
  mqtt.begin();
  device.connect();
  device.publishExtraButtons();
  stepperDeviceConnection.connect();
  stepperDeviceConnection.publishExtraButtons();     
}

void loop() {
  mqtt.loop();                  // MQTT client loop
  stepperDevice.loop();        // Your non-blocking stepper updates
}

