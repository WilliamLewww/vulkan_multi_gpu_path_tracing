#pragma once

#include "bottom_level_acceleration_structure.h"
#include "top_level_acceleration_structure.h"
#include "model_instance.h"

class AccelerationStructureSet {
private:
  std::vector<BottomLevelAccelerationStructure*> bottomLevelAccelerationStructureList;
  std::vector<VkAccelerationStructureInstanceKHR> bottomLevelAccelerationStructureInstanceList;

  TopLevelAccelerationStructure* topLevelAccelerationStructure;

  VkWriteDescriptorSetAccelerationStructureKHR writeDescriptorSetAccelerationStructure;

  void addBottomLevelAccelerationStructureInstance(BottomLevelAccelerationStructure* bottomLevelAccelerationStructure, ModelInstance* modelInstance, VkDevice logicalDevice);
public:
  AccelerationStructureSet(std::map<Model*, std::vector<ModelInstance*>> modelInstanceMap, 
                                  VkDevice logicalDevice, 
                                  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                  VkCommandPool commandPool,
                                  VkQueue queue);

  ~AccelerationStructureSet();

  VkWriteDescriptorSetAccelerationStructureKHR* getWriteDescriptorSetAccelerationStructurePointer();

  void updateAccelerationStructure(std::vector<ModelInstance*> modelInstanceList,
                                   VkDevice logicalDevice, 
                                   VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                   VkCommandPool commandPool,
                                   VkQueue queue);
};