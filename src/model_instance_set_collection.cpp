#include "model_instance_set_collection.h"

ModelInstanceSetCollection::ModelInstanceSetCollection(std::vector<std::map<Model*, std::vector<TRS>>> modelFrequencyMapList,
                                                 VkDevice logicalDevice, 
                                                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                 VkCommandPool commandPool,
                                                 VkQueue queue) {

  for (int x = 0; x < modelFrequencyMapList.size(); x++) {
    this->modelInstanceSetList.push_back(new ModelInstanceSet(modelFrequencyMapList[x],
                                                              logicalDevice, 
                                                              physicalDeviceMemoryProperties, 
                                                              commandPool,
                                                              queue));

    for (int y = 0; y < this->modelInstanceSetList[x]->getInstanceCount(); y++) {
      this->collectionIndexList.push_back(x);
    }

    this->collectionOffsetList.push_back(this->modelInstanceSetList[x]->getInstanceCount());
  }

  VkDeviceSize bufferSize = sizeof(uint32_t) * this->collectionIndexList.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              bufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &stagingBuffer, 
                              &stagingBufferMemory);

  void* data;
  vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, this->collectionIndexList.data(), bufferSize);
  vkUnmapMemory(logicalDevice, stagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              bufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->collectionIndexBuffer, 
                              &this->collectionIndexBufferMemory);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, stagingBuffer, this->collectionIndexBuffer, bufferSize);

  vkDestroyBuffer(logicalDevice, stagingBuffer, NULL);
  vkFreeMemory(logicalDevice, stagingBufferMemory, NULL);

  this->descriptorCollectionIndexBufferInfo = {
    .buffer = this->collectionIndexBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
}

ModelInstanceSetCollection::~ModelInstanceSetCollection() {

}

std::vector<std::map<Model*, std::vector<ModelInstance*>>> ModelInstanceSetCollection::getModelInstanceMapList() {
  std::vector<std::map<Model*, std::vector<ModelInstance*>>> modelInstanceMapList;

  for (int x = 0; x < this->modelInstanceSetList.size(); x++) {
    modelInstanceMapList.push_back(this->modelInstanceSetList[x]->getModelInstanceMap());
  }

  return modelInstanceMapList;
}

ModelInstanceSet* ModelInstanceSetCollection::getModelInstanceSet(int index) {
  return this->modelInstanceSetList[index];
}

VkDescriptorBufferInfo* ModelInstanceSetCollection::getDescriptorCollectionIndexBufferInfoPointer() {
  return &this->descriptorCollectionIndexBufferInfo;
}