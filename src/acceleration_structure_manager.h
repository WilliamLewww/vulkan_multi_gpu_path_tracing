#pragma once

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

#include "buffer.h"
#include "device.h"

class AccelerationStructureManager {
private:
  std::vector<VkAccelerationStructureKHR> bottomLevelAccelerationStructureList;
  std::vector<VkBuffer> bottomLevelAccelerationStructureBufferList;
  std::vector<VkDeviceMemory> bottomLevelAccelerationStructureMemoryList;
public:
  AccelerationStructureManager();
  ~AccelerationStructureManager();

  void createBottomLevelAccelerationStructure(Device device, VkCommandPool commandPool, VkQueue computeQueue, uint32_t primitiveCount, uint32_t vertexCount, VkBuffer vertexBuffer, VkBuffer indexBuffer);
};