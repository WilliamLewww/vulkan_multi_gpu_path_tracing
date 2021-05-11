#pragma once

#include <vulkan/vulkan.h>
#include <string.h>

#include "buffer_factory.h"

class UniformBuffer {
private:
  VkBuffer uniformBuffer;
  VkDeviceMemory uniformBufferMemory;

  VkDescriptorBufferInfo descriptorBufferInfo;
public:
  UniformBuffer(VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, void* buffer, uint32_t bufferSize);
  ~UniformBuffer();

  VkDeviceMemory getDeviceMemory();
  VkDescriptorBufferInfo* getDescriptorBufferInfoPointer();

  void updateUniformBuffer(VkDevice logicalDevice, void* buffer, uint32_t bufferSize);
};