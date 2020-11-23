#pragma once

#include "bottom_level_acceleration_structure.h"
#include "top_level_acceleration_structure.h"
#include "model_instance.h"

class AccelerationStructureCollection {
private:
  std::vector<BottomLevelAccelerationStructure*> bottomLevelAccelerationStructureList;
  std::vector<VkAccelerationStructureInstanceKHR> bottomLevelAccelerationStructureInstanceList;

  TopLevelAccelerationStructure* topLevelAccelerationStructure;

  void addBottomLevelAccelerationStructureInstance(BottomLevelAccelerationStructure* bottomLevelAccelerationStructure, ModelInstance* modelInstance, VkDevice logicalDevice);
public:
  AccelerationStructureCollection(std::map<Model*, std::vector<ModelInstance*>> modelInstanceMap, 
                                  VkDevice logicalDevice, 
                                  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                  VkCommandPool commandPool,
                                  VkQueue queue);

  ~AccelerationStructureCollection();
};