#pragma once
#include <Arduino.h>
#include <AccelStepper.h>
#include "interfaces/IDevice.h"
#include "interfaces/HADevice.h"
#include "EEPROMSettings.h"  // include new class

class StepperDevice : public IDevice {
private:
          AccelStepper stepper;
          uint8_t pin1, pin2, pin3, pin4;
          int stepOn;
          int stepOff;
          int currentStep = 0;
          HADeviceConnection* device = nullptr;
          ServoSettingsStorage storage; // Reuse for storing stepOn/stepOff

public:
          StepperDevice(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4, int defaultOn = 512, int defaultOff = 0)
                    : stepper(AccelStepper::HALF4WIRE, in1, in3, in2, in4),
                    pin1(in1), pin2(in2), pin3(in3), pin4(in4),
                    stepOn(defaultOn), stepOff(defaultOff)
          {
                    storage.begin(); // init EEPROM
                    stepOn = storage.loadAngleOn(defaultOn);   // reuse for step
                    stepOff = storage.loadAngleOff(defaultOff); // reuse for step
                    Serial.print("Step ON: "); Serial.print(stepOn);
                    Serial.print(" | Step OFF: "); Serial.println(stepOff);

                    stepper.setMaxSpeed(1000);
                    stepper.setAcceleration(200);
                    stepper.setCurrentPosition(0);
          }
          bool isMoving = false;
          String pendingState = "";

          void loop() {
                    if (isMoving) {
                              stepper.run();  // This must be called as frequently as possible
                              if (stepper.distanceToGo() == 0) {
                                        isMoving = false;
                                        currentStep = stepper.currentPosition();
                                        Serial.println("Stepper reached target: " + String(currentStep));
                                        if (device && pendingState.length() > 0) {
                                                  device->publishState(pendingState);
                                                  pendingState = "";
                                        }
                              }
                    }
          }
                
                

          void setDeviceConnection(HADeviceConnection& device) override {
                    this->device = &device;
          }

          void processCommandCallback(const String& cmd) override {
                    if (!device) {
                              Serial.println("Device not set for StepperDevice, cannot handle command.");
                              return;
                    }
                    else {
                              Serial.println("Handling command: " + cmd);
                    }

                    int targetStep = currentStep;
                    String newState;

                    if (cmd.equalsIgnoreCase("ON")) {
                              targetStep = stepOff;  // ⬅️ flip
                              newState = "ON";
                    }
                    else if (cmd.equalsIgnoreCase("OFF")) {
                              targetStep = stepOn;   // ⬅️ flip
                              newState = "OFF";
                    }
                          
                    else if (cmd.startsWith("+") || cmd.startsWith("-")) {
                              int delta = cmd.toInt();  // '+' or '-' delta movement
                              targetStep = currentStep + (delta*15);
                              newState = String(targetStep);
                    }
                    else if (cmd.startsWith("SET_ON")) {
                              stepOff = currentStep;                     // ⬅️ store current as OFF
                              storage.saveAngleOff(stepOff);
                              Serial.println("New OFF step set to: " + String(stepOff));
                              return;
                    }
                    else if (cmd.startsWith("SET_OFF")) {
                              stepOn = currentStep;                      // ⬅️ store current as ON
                              storage.saveAngleOn(stepOn);
                              Serial.println("New ON step set to: " + String(stepOn));
                              return;
                    }
                          
                    else if (cmd.toInt() != 0 || cmd == "0") {
                              targetStep = cmd.toInt();
                              newState = String(targetStep);
                    }
                    else {
                              Serial.println("Unknown command: " + cmd);
                              return;
                    }

                    if (targetStep != currentStep) {
                              Serial.println("Moving stepper to: " + String(targetStep));
                              stepper.moveTo(targetStep);
                              isMoving = true;
                              pendingState = newState;  // store what we’ll publish later
                    }
                    else {
                              Serial.println("Command matches current state, ignoring.");
                    }
                          
          }
};
