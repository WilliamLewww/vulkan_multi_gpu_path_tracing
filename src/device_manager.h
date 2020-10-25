#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "physical_device.h"

class DeviceManager {
private:
  std::vector<PhysicalDevice> physicalDeviceList;
public:
  DeviceManager(VkInstance vulkanInstance);
  ~DeviceManager();
};