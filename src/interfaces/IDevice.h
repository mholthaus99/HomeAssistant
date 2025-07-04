// interfaces/ICommandHandler.h
#pragma once
#include <Arduino.h>


class IDevice {
public:
     virtual void processCommandCallback(const String& cmd) = 0;
     virtual void setDeviceConnection(class HADeviceConnection& deviceConnection) = 0; // Set the device for this handler
     virtual ~IDevice() {}
};
