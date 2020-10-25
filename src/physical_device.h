#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class PhysicalDevice {
private:
  VkPhysicalDevice device;
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
public:
  PhysicalDevice(VkPhysicalDevice device);
  ~PhysicalDevice();
};