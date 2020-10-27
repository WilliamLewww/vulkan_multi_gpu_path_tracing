#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "device.h"
#include "display_device.h"

class DeviceManager {
private:
  std::vector<Device*> deviceList;
  DisplayDevice* displayDevice;
public:
  DeviceManager(VkInstance vulkanInstance);
  ~DeviceManager();

  int getDeviceCount();
  Device* getDeviceAddressFromIndex(int x);

  DisplayDevice* getDisplayDeviceAddress();
};