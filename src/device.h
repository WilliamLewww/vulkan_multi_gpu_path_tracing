#pragma once

#include <vulkan/vulkan.h>

class Device {
private:
  VkPhysicalDevice physicalDevice;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  VkPhysicalDeviceFeatures physicalDeviceFeatures;
  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

  VkDevice logicalDevice;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();
};