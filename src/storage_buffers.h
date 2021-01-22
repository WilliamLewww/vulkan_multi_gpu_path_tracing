#pragma once

#include <string.h>

#include "buffer_factory.h"

class StorageBuffers {
private:
  struct LensPropertiesUniform {
    alignas(4) uint32_t apertureInstanceIndex;
    alignas(4) uint32_t aperturePrimitiveCount;
    alignas(4) uint32_t aperturePrimitiveOffset;
  };

  VkBuffer rayDirectionBuffer;
  VkDeviceMemory rayDirectionBufferMemory;

  VkBuffer lightDepthBuffer;
  VkDeviceMemory lightDepthBufferMemory;

  VkBuffer lensPropertiesBuffer;
  VkDeviceMemory lensPropertiesBufferMemory;

  VkDescriptorBufferInfo descriptorRayDirectionBufferInfo;
  VkDescriptorBufferInfo descriptorLightDepthBufferInfo;
  VkDescriptorBufferInfo descriptorLensPropertiesBufferInfo;
public:
  StorageBuffers(uint32_t apertureInstanceIndex,
                 uint32_t aperturePrimitiveCount,
                 uint32_t aperturePrimitiveOffset,
                 VkDevice logicalDevice, 
                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                 VkCommandPool commandPool,
                 VkQueue queue);
  ~StorageBuffers();

  VkDescriptorBufferInfo* getDescriptorRayDirectionBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorLensPropertiesBufferInfoPointer();
};