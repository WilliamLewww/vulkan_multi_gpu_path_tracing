#pragma once
#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <map>
#include <stdio.h>

#include "device_queue.h"

class Device {
private:
  VkPhysicalDevice physicalDevice;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  VkPhysicalDeviceFeatures physicalDeviceFeatures;
  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

  VkDevice logicalDevice;

  DeviceQueue* deviceQueue;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();

  void initializeDeviceQueue(VkSurfaceKHR surface);
  void createLogicalDevice(std::vector<const char*> extensions);
};