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
  std::vector<VkDeviceMemory> bottomLevelAccelerationStructureDeviceMemoryList;

  VkAccelerationStructureKHR topLevelAccelerationStructure;
  VkBuffer topLevelAccelerationStructureBuffer;
  VkDeviceMemory topLevelAccelerationStructureDeviceMemory;
public:
  AccelerationStructureManager();
  ~AccelerationStructureManager();

  VkAccelerationStructureKHR* getTopLevelAccelerationStructurePointer();

  void createBottomLevelAccelerationStructure(Device* device, uint32_t primitiveCount, uint32_t vertexCount, VkBuffer vertexBuffer, VkBuffer indexBuffer);
  void createTopLevelAccelerationStructure(Device* device);
};