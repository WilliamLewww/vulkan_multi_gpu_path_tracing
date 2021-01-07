#pragma once

#include <string.h>

#include "buffer_factory.h"

class StorageBuffers {
private:
  struct LensPropertiesUniform {
    alignas(4) uint32_t apertureInstanceIndex;
    alignas(4) uint32_t aperturePrimitiveCount;
  };

  VkBuffer rayDirectionBuffer;
  VkDeviceMemory rayDirectionBufferMemory;

  VkBuffer lensPropertiesBuffer;
  VkDeviceMemory lensPropertiesBufferMemory;

  VkDescriptorBufferInfo descriptorRayDirectionBufferInfo;
public:
  StorageBuffers(uint32_t apertureInstanceIndex,
                 uint32_t aperturePrimitiveCount,
                 VkDevice logicalDevice, 
                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                 VkCommandPool commandPool,
                 VkQueue queue);
  ~StorageBuffers();

  VkDescriptorBufferInfo* getDescriptorRayDirectionBufferInfoPointer();
};