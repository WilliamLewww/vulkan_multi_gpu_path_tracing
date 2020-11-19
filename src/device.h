#pragma once
#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <map>
#include <stdio.h>

#include "device_queue.h"
#include "device_command_pool.h"
#include "device_swapchain.h"

class Device {
private:
  VkPhysicalDevice physicalDevice;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  VkPhysicalDeviceFeatures physicalDeviceFeatures;
  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

  VkDevice logicalDevice;

  DeviceQueue* deviceQueue;
  DeviceCommandPool* deviceCommandPool;
  DeviceSwapchain* deviceSwapchain;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();

  void initializeDeviceQueue(VkSurfaceKHR surface);
  void createLogicalDevice(std::vector<const char*> extensions);
  void createDeviceCommandPool();
  void createDeviceSwapchain(VkSurfaceKHR surface);
};