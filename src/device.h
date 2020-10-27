#pragma once

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdio.h>

class Device {
protected:
  VkPhysicalDevice physicalDevice;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  VkPhysicalDeviceFeatures physicalDeviceFeatures;

  VkDevice logicalDevice;

  int graphicsQueueIndex;
  int presentQueueIndex;
  int computeQueueIndex;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VkQueue computeQueue;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();

  void initializeQueues(VkSurfaceKHR surface);
  void createLogicalDevice(std::vector<const char*> extensions);
  void createSwapchain(VkSurfaceKHR surface);
};