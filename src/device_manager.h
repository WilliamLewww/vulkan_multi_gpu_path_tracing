#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "device.h"

class DeviceManager {
private:
  std::vector<Device> deviceList;
public:
  DeviceManager(VkInstance vulkanInstance);
  ~DeviceManager();
};