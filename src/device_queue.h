#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <stdio.h>

class DeviceQueue {
private:
  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList;

  uint32_t graphicsQueueIndex;
  uint32_t presentQueueIndex;
  uint32_t computeQueueIndex;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VkQueue computeQueue;

  std::map<uint32_t, uint32_t> queueFrequencyMap;
public:
  DeviceQueue(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
  ~DeviceQueue();

  uint32_t getGraphicsQueueIndex();
  uint32_t getPresentQueueIndex();
  uint32_t getComputeQueueIndex();

  std::map<uint32_t, uint32_t> getQueueFrequencyMap();

  void setQueueHandles(VkDevice logicalDevice);
};