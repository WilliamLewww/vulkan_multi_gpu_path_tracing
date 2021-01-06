#pragma once

#include "model_instance_set.h"

class ModelInstanceSetCollection {
private:
  std::vector<ModelInstanceSet*> modelInstanceSetList;

  std::vector<uint32_t> collectionIndexList;
  std::vector<uint32_t> collectionOffsetList;

  VkBuffer collectionIndexBuffer;
  VkDeviceMemory collectionIndexBufferMemory;

  VkBuffer collectionOffsetBuffer;
  VkDeviceMemory collectionOffsetBufferMemory;

  VkDescriptorBufferInfo descriptorCollectionIndexBufferInfo;
  VkDescriptorBufferInfo descriptorCollectionOffsetBufferInfo;
public:
  ModelInstanceSetCollection(std::vector<std::map<Model*, std::vector<TRS>>> modelFrequencyMapList,
                          VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue);
  ~ModelInstanceSetCollection();

  std::vector<std::map<Model*, std::vector<ModelInstance*>>> getModelInstanceMapList();

  uint32_t getCollectionOffset(int index);

  ModelInstanceSet* getModelInstanceSet(int index);
  VkDescriptorBufferInfo* getDescriptorCollectionIndexBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorCollectionOffsetBufferInfoPointer();
};