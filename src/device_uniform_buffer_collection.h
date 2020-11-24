#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>

#include "device_uniform_buffer.h"

class DeviceUniformBufferCollection {
private:
  std::vector<DeviceUniformBuffer*> deviceUniformBufferList;
public:
  DeviceUniformBufferCollection(std::map<void*, uint32_t> bufferMap, VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties);
  ~DeviceUniformBufferCollection();

  void updateUniformBuffer(VkDevice logicalDevice, int index, void* buffer, uint32_t bufferSize);
  VkDescriptorBufferInfo* getDescriptorBufferInfoPointer(int index);
};