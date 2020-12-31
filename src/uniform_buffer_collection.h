#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>

#include "uniform_buffer.h"

class UniformBufferCollection {
private:
  std::vector<UniformBuffer*> uniformBufferList;
public:
  UniformBufferCollection(std::map<void*, uint32_t> bufferMap, VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties);
  ~UniformBufferCollection();

  void updateUniformBuffer(VkDevice logicalDevice, int index, void* buffer, uint32_t bufferSize);
  VkDescriptorBufferInfo* getDescriptorBufferInfoPointer(int index);
};