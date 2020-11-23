#pragma once
#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <vector>
#include <string.h>

#include "buffer_factory.h"

class TopLevelAccelerationStructure {
private:
    VkAccelerationStructureKHR accelerationStructure;
    VkBuffer accelerationStructureBuffer;
    VkDeviceMemory accelerationStructureBufferMemory;
public:
  TopLevelAccelerationStructure(std::vector<VkAccelerationStructureInstanceKHR> bottomLevelAccelerationStructureInstanceList,
                                VkDevice logicalDevice, 
                                VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                VkCommandPool commandPool,
                                VkQueue queue);

  ~TopLevelAccelerationStructure();
};