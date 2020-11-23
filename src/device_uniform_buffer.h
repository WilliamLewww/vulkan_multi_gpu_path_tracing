#pragma once

#include <vulkan/vulkan.h>
#include <string.h>

#include "buffer_factory.h"

class DeviceUniformBuffer {
private:
  VkBuffer uniformBuffer;
  VkDeviceMemory uniformBufferMemory;
public:
  DeviceUniformBuffer(VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, void* buffer, uint32_t bufferSize);
  ~DeviceUniformBuffer();
};