#pragma once

#include "model_instance_set.h"

class ModelInstanceSetCollection {
private:
  std::vector<ModelInstanceSet*> modelInstanceSetList;

  std::vector<uint32_t> collectionIndexList;
  std::vector<uint32_t> collectionOffsetList;

  VkBuffer collectionIndexBuffer;
  VkDeviceMemory collectionIndexBufferMemory;

  VkDescriptorBufferInfo descriptorCollectionIndexBufferInfo;
public:
  ModelInstanceSetCollection(std::vector<std::map<Model*, std::vector<TRS>>> modelFrequencyMapList,
                          VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue);
  ~ModelInstanceSetCollection();

  std::vector<std::map<Model*, std::vector<ModelInstance*>>> getModelInstanceMapList();

  ModelInstanceSet* getModelInstanceSet(int index);
  VkDescriptorBufferInfo* getDescriptorCollectionIndexBufferInfoPointer();
};