#pragma once
#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>

#include "model.h"
#include "buffer_factory.h"

class BottomLevelAccelerationStructure {
private:
  VkAccelerationStructureKHR accelerationStructure;
  VkBuffer accelerationStructureBuffer;
  VkDeviceMemory accelerationStructureBufferMemory;
public:
  BottomLevelAccelerationStructure(Model* model,
                                   VkBuffer vertexBuffer,
                                   VkBuffer indexBuffer,
                                   VkDevice logicalDevice, 
                                   VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                   VkCommandPool commandPool,
                                   VkQueue queue);

  ~BottomLevelAccelerationStructure();

  VkAccelerationStructureKHR getAccelerationStructure();
};