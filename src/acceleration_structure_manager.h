#pragma once

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

#include "buffer.h"
#include "device.h"

class AccelerationStructureManager {
private:
  struct DeviceContainer {
    std::vector<VkAccelerationStructureKHR> bottomLevelAccelerationStructureList;
    std::vector<VkBuffer> bottomLevelAccelerationStructureBufferList;
    std::vector<VkDeviceMemory> bottomLevelAccelerationStructureDeviceMemoryList;

    VkAccelerationStructureKHR topLevelAccelerationStructure;
    VkBuffer topLevelAccelerationStructureBuffer;
    VkDeviceMemory topLevelAccelerationStructureDeviceMemory;

    std::vector<VkAccelerationStructureInstanceKHR> bottomLevelAccelerationStructureInstanceList;
  };
  std::map<Device*, DeviceContainer> deviceMap;
public:
  AccelerationStructureManager();
  ~AccelerationStructureManager();

  VkAccelerationStructureKHR* getTopLevelAccelerationStructurePointer(Device* device);

  void initializeContainerOnDevice(Device* device);
  void createBottomLevelAccelerationStructure(Device* device, uint32_t primitiveCount, uint32_t vertexCount, VkBuffer vertexBuffer, VkBuffer indexBuffer);
  void addBottomLevelAccelerationStructureInstance(Device* device, uint32_t bottomLevelAccelerationStructureIndex, uint32_t instanceIndex, VkTransformMatrixKHR transformMatrix);
  
  void createTopLevelAccelerationStructure(Device* device);
};