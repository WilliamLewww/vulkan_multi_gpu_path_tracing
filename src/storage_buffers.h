#pragma once

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

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

  VkBuffer randomBuffer;
  VkDeviceMemory randomBufferMemory;

  VkBuffer rayDirectionBuffer;
  VkDeviceMemory rayDirectionBufferMemory;

  VkBuffer lensPropertiesBuffer;
  VkDeviceMemory lensPropertiesBufferMemory;

  VkBuffer flareBuffer;
  VkDeviceMemory flareBufferMemory;

  VkDescriptorBufferInfo descriptorRandomBufferInfo;
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

  VkDescriptorBufferInfo* getDescriptorRandomBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorRayDirectionBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorLensPropertiesBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorFlareBufferInfoPointer();
};