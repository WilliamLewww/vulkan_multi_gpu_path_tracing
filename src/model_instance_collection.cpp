#include "model_instance_collection.h"

ModelInstanceCollection::ModelInstanceCollection(std::map<Model*, uint32_t> modelFrequencyMap, 
                                                 VkDevice logicalDevice, 
                                                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                 VkCommandPool commandPool,
                                                 VkQueue queue) {
  int modelIndex = 0;
  int instanceIndex = 0;
  for (std::pair<Model*, uint32_t> pair : modelFrequencyMap) {
    this->createVertexBuffer(pair.first, logicalDevice, physicalDeviceMemoryProperties, commandPool, queue);
    this->createIndexBuffer(pair.first, logicalDevice, physicalDeviceMemoryProperties, commandPool, queue);

    for (int x = 0; x < pair.second; x++) {
      this->modelInstanceList.push_back(new ModelInstance(pair.first, modelIndex, instanceIndex));
    }
    modelIndex += 1;
  }
}

ModelInstanceCollection::~ModelInstanceCollection() {
  for (int x = 0; x < this->modelInstanceList.size(); x++) {
    delete this->modelInstanceList[x];
  }
}

void ModelInstanceCollection::createVertexBuffer(Model* model,
                                                 VkDevice logicalDevice, 
                                                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                 VkCommandPool commandPool,
                                                 VkQueue queue) {

  VkDeviceSize bufferSize = sizeof(float) * model->getVertexCount();
  
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
  memcpy(data, model->getVertices().data(), bufferSize);
  vkUnmapMemory(logicalDevice, stagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              bufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->vertexBufferMap[model], 
                              &this->vertexBufferMemoryMap[model]);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, stagingBuffer, this->vertexBufferMap[model], bufferSize);

  vkDestroyBuffer(logicalDevice, stagingBuffer, NULL);
  vkFreeMemory(logicalDevice, stagingBufferMemory, NULL);
}

void ModelInstanceCollection::createIndexBuffer(Model* model,
                                                VkDevice logicalDevice, 
                                                VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                VkCommandPool commandPool,
                                                VkQueue queue) {

  VkDeviceSize bufferSize = sizeof(uint32_t) * model->getTotalIndexCount();

  std::vector<uint32_t> positionIndexList(model->getTotalIndexCount());
  for (int x = 0; x < model->getTotalIndexCount(); x++) {
    positionIndexList[x] = model->getTotalIndex(x).vertex_index;
  }
  
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
  memcpy(data, positionIndexList.data(), bufferSize);
  vkUnmapMemory(logicalDevice, stagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              bufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->indexBufferMap[model], 
                              &this->indexBufferMemoryMap[model]);

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, stagingBuffer, this->indexBufferMap[model], bufferSize);
  
  vkDestroyBuffer(logicalDevice, stagingBuffer, NULL);
  vkFreeMemory(logicalDevice, stagingBufferMemory, NULL);
}

void ModelInstanceCollection::createMaterialBuffers(Model* model,
                                                    VkDevice logicalDevice, 
                                                    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                    VkCommandPool commandPool,
                                                    VkQueue queue) {

}