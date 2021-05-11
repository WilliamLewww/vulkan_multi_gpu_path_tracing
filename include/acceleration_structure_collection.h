#pragma once

#include "acceleration_structure_set.h"

class AccelerationStructureCollection {
private:
  std::vector<AccelerationStructureSet*> accelerationStructureSetList;
public:
  AccelerationStructureCollection(std::vector<std::map<Model*, std::vector<ModelInstance*>>> modelInstanceMapList, 
                                  VkDevice logicalDevice, 
                                  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                  VkCommandPool commandPool,
                                  VkQueue queue);
  ~AccelerationStructureCollection();

  AccelerationStructureSet* getAccelerationStructureSet(int index);
};