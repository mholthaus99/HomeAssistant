#pragma once
#include <EEPROM.h>

class ServoSettingsStorage {
public:
          static const int EEPROM_SIZE = 512;
          static const int ADDR_ANGLE_ON = 0;
          static const int ADDR_ANGLE_OFF = sizeof(int);

          void begin() {
                    EEPROM.begin(EEPROM_SIZE);
          }

          int loadAngleOn(int fallback = 80) {
                    int val;
                    EEPROM.get(ADDR_ANGLE_ON, val);
                    return (val >= 0 && val <= 180) ? val : fallback;
          }

          int loadAngleOff(int fallback = 15) {
                    int val;
                    EEPROM.get(ADDR_ANGLE_OFF, val);
                    return (val >= 0 && val <= 180) ? val : fallback;
          }

          void saveAngleOn(int val) {
                    EEPROM.put(ADDR_ANGLE_ON, val);
                    EEPROM.commit();
          }

          void saveAngleOff(int val) {
                    EEPROM.put(ADDR_ANGLE_OFF, val);
                    EEPROM.commit();
          }
};
