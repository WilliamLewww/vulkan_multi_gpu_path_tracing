#pragma once

#include "buffer_factory.h"

class StorageBuffers {
private:
  VkBuffer rayDirectionBuffer;
  VkDeviceMemory rayDirectionBufferMemory;

  VkDescriptorBufferInfo descriptorRayDirectionBufferInfo;
public:
  StorageBuffers(VkDevice logicalDevice, 
                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                 VkCommandPool commandPool,
                 VkQueue queue);
  ~StorageBuffers();

  VkDescriptorBufferInfo* getDescriptorRayDirectionBufferInfoPointer();
};