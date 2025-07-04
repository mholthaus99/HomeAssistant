#pragma once
#include <Servo.h>
#include <Arduino.h>
#include "interfaces/IDevice.h"
#include "interfaces/HADevice.h"
#include "EEPROMSettings.h"  // include new class

class ServoDevice : public IDevice {
private:
    Servo servo;
    uint8_t pin;
    int angleOn;
    int angleOff;
    int currentAngle = -1;
    HADeviceConnection* device = nullptr;
    ServoSettingsStorage storage;

public:
    ServoDevice(uint8_t pin, int defaultOn = 80, int defaultOff = 15)
        : pin(pin), angleOn(defaultOn), angleOff(defaultOff) {
        storage.begin(); // init EEPROM
        angleOn = storage.loadAngleOn(defaultOn);
        angleOff = storage.loadAngleOff(defaultOff);
        Serial.print(angleOn); Serial.println(angleOff);
        servo.attach(pin, 500, 2400); // Attach with min/max pulse width
    }

    void setDeviceConnection(HADeviceConnection& device) override {
        this->device = &device;
    }

    void processCommandCallback(const String& cmd) override {
        if (!device) {
            Serial.println("Device not set for ServoCommandHandler, cannot handle command.");
            return;
        }
        else {
            Serial.println("Handling command: " + cmd);
        }

        int targetAngle = currentAngle;
        String newState;

        if (cmd.equalsIgnoreCase("ON")) {
            targetAngle = angleOn;
            newState = "ON";
        }
        else if (cmd.equalsIgnoreCase("OFF")) {
            targetAngle = angleOff;
            newState = "OFF";
        }
        else if (cmd.startsWith("+") || cmd.startsWith("-")) {
            int delta = cmd.toInt();
            targetAngle = constrain(currentAngle + delta, 0, 180);
            newState = String(targetAngle);
        }
        else if (cmd.startsWith("SET_ON")) {
            //int val = constrain(cmd.substring(7).toInt(), 0, 180);
            angleOn = currentAngle;
            storage.saveAngleOn(angleOn);
            Serial.println("New ON angle set to: " + String(angleOn));
            return;
        }
        else if (cmd.startsWith("SET_OFF")) {
            //int val = constrain(cmd.substring(8).toInt(), 0, 180);
            angleOff = currentAngle;
            storage.saveAngleOff(angleOff);
            Serial.println("New OFF angle set to: " + String(angleOff));
            return;
        }
        else if (cmd.toInt() >= 0 && cmd.toInt() <= 180) {
            targetAngle = cmd.toInt();
            newState = String(targetAngle);
        }
        else {
            Serial.println("Unknown command: " + cmd);
            return;
        }

        if (targetAngle != currentAngle) {
            Serial.println("Moving servo to: " + String(targetAngle));
            servo.attach(pin, 500, 2400);
            servo.write(targetAngle);
            int delayTime = abs(targetAngle - currentAngle) * 10;
            delay(constrain(delayTime, 200, 1000));
            servo.detach();
            currentAngle = targetAngle;
            device->publishState(newState);
        }
        else {
            Serial.println("Command matches current state, ignoring.");
        }
    }
};
