#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>

class DeviceInfo {
public:
     static String getSanitizedMac() {
          String mac = WiFi.macAddress();   // "84:F3:EB:12:34:56"
          mac.replace(":", "");             // "84F3EB123456"
          mac.toLowerCase();                // "84f3eb123456"
          return mac;
     }

     static String getDeviceId(const String& prefix = "esp") {
          return prefix + "_" + getSanitizedMac();  // "esp_84f3eb123456"
     }
};
