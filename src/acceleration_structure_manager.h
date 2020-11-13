#pragma once

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <map>
#include <vector>
#include <stdio.h>

#include "buffer.h"
#include "device.h"
#include "model.h"
#include "instance_manager.h"

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
    std::vector<VkTransformMatrixKHR> instanceTransformMatrixList;
  };
  std::map<Device*, DeviceContainer> deviceMap;
public:
  AccelerationStructureManager();
  ~AccelerationStructureManager();

  VkAccelerationStructureKHR* getTopLevelAccelerationStructurePointer(Device* device);

  void initializeContainerOnDevice(Device* device);
  void createBottomLevelAccelerationStructure(Device* device, Model* model);
  void addBottomLevelAccelerationStructureInstance(Device* device, ModelInstance modelInstance);
  
  void createTopLevelAccelerationStructure(Device* device);
};