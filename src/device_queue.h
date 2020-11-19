#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class DeviceQueue {
private:
  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList;

  uint32_t graphicsQueueIndex;
  uint32_t presentQueueIndex;
  uint32_t computeQueueIndex;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VkQueue computeQueue;
public:
  DeviceQueue(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
  ~DeviceQueue();

  uint32_t getGraphicsQueueIndex();
  uint32_t getPresentQueueIndex();
  uint32_t getComputeQueueIndex();

  void setQueueHandles(VkDevice logicalDevice);
};