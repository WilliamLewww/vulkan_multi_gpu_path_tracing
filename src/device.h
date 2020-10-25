#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class Device {
private:
  VkPhysicalDevice physicalDevice;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  VkPhysicalDeviceFeatures physicalDeviceFeatures;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();
};