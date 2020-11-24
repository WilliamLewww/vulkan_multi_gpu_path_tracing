#pragma once

#include <vulkan/vulkan.h>
#include <string.h>

#include "buffer_factory.h"

class DeviceUniformBuffer {
private:
  VkBuffer uniformBuffer;
  VkDeviceMemory uniformBufferMemory;

  VkDescriptorBufferInfo descriptorBufferInfo;
public:
  DeviceUniformBuffer(VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, void* buffer, uint32_t bufferSize);
  ~DeviceUniformBuffer();

  VkDeviceMemory getDeviceMemory();
  VkDescriptorBufferInfo* getDescriptorBufferInfoPointer();

  void updateUniformBuffer(VkDevice logicalDevice, void* buffer, uint32_t bufferSize);
};