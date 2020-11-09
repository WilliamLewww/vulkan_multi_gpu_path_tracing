#pragma once

#include <vulkan/vulkan.h>
#include <stdio.h>

#include "device.h"

class Buffer {
public:
  static void createBuffer(Device device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
};