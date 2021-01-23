#pragma once

#include <string.h>

#include "buffer_factory.h"

class StorageBuffers {
private:
  struct LensPropertiesUniform {
    alignas(4) uint32_t apertureInstanceIndex;
    alignas(4) uint32_t aperturePrimitiveCount;
    alignas(4) uint32_t aperturePrimitiveOffset;
    alignas(4) uint32_t lastLensElementInstanceIndex;
    alignas(4) uint32_t lastLensElementPrimitiveCount;
    alignas(4) uint32_t filmInstanceIndex;
  };

  VkBuffer rayDirectionBuffer;
  VkDeviceMemory rayDirectionBufferMemory;

  VkBuffer lensPropertiesBuffer;
  VkDeviceMemory lensPropertiesBufferMemory;

  VkBuffer flareBuffer;
  VkDeviceMemory flareBufferMemory;

  VkDescriptorBufferInfo descriptorRayDirectionBufferInfo;
  VkDescriptorBufferInfo descriptorLensPropertiesBufferInfo;
  VkDescriptorBufferInfo descriptorFlareBufferInfo;
public:
  StorageBuffers(uint32_t apertureInstanceIndex,
                 uint32_t aperturePrimitiveCount,
                 uint32_t aperturePrimitiveOffset,
                 uint32_t lastLensElementInstanceIndex,
                 uint32_t lastLensElementPrimitiveCount,
                 uint32_t filmInstanceIndex,
                 VkDevice logicalDevice, 
                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                 VkCommandPool commandPool,
                 VkQueue queue);
  ~StorageBuffers();

  VkDescriptorBufferInfo* getDescriptorRayDirectionBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorLensPropertiesBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorFlareBufferInfoPointer();
};