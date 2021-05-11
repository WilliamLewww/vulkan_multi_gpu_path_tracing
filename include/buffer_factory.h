#pragma once

#include <vulkan/vulkan.h>
#include <stdio.h>

class BufferFactory {
public:
  static void createBuffer(VkDevice logicalDevice,
                           VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties,
                           VkDeviceSize size, 
                           VkBufferUsageFlags usageFlags, 
                           VkMemoryPropertyFlags propertyFlags, 
                           VkBuffer* buffer, 
                           VkDeviceMemory* bufferMemory);

  static void copyBuffer(VkDevice logicalDevice,
                         VkCommandPool commandPool,
                         VkQueue queue,
                         VkBuffer srcBuffer,
                         VkBuffer dstBuffer, 
                         VkDeviceSize size);
};