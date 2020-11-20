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

  VkDeviceSize positionBufferSize = sizeof(float) * model->getVertexCount();
  
  VkBuffer positionStagingBuffer;
  VkDeviceMemory positionStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              positionBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &positionStagingBuffer, 
                              &positionStagingBufferMemory);

  void* positionData;
  vkMapMemory(logicalDevice, positionStagingBufferMemory, 0, positionBufferSize, 0, &positionData);
  memcpy(positionData, model->getVertices().data(), positionBufferSize);
  vkUnmapMemory(logicalDevice, positionStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              positionBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->vertexBufferMap[model], 
                              &this->vertexBufferMemoryMap[model]);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, positionStagingBuffer, this->vertexBufferMap[model], positionBufferSize);

  vkDestroyBuffer(logicalDevice, positionStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, positionStagingBufferMemory, NULL);
}

void ModelInstanceCollection::createIndexBuffer(Model* model,
                                                VkDevice logicalDevice, 
                                                VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                VkCommandPool commandPool,
                                                VkQueue queue) {

}

void ModelInstanceCollection::createMaterialBuffers(Model* model,
                                                    VkDevice logicalDevice, 
                                                    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                    VkCommandPool commandPool,
                                                    VkQueue queue) {

}