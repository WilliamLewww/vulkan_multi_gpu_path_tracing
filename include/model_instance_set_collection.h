#pragma once

#include "model_instance_set.h"

class ModelInstanceSetCollection {
private:
  std::vector<ModelInstanceSet*> modelInstanceSetList;
public:
  ModelInstanceSetCollection(std::vector<std::map<Model*, std::vector<TRS>>> modelFrequencyMapList,
                          VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue);
  ~ModelInstanceSetCollection();

  std::vector<std::map<Model*, std::vector<ModelInstance*>>> getModelInstanceMapList();

  uint32_t getModelInstanceSetCount();
  ModelInstanceSet* getModelInstanceSet(int index);
};