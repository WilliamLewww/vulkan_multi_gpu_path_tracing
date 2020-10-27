#pragma once

#include "device.h"

class DisplayDevice : public Device {
private:
public:
  DisplayDevice(VkPhysicalDevice physicalDevice);
  ~DisplayDevice();
};