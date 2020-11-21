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
    this->createMaterialBuffers(pair.first, logicalDevice, physicalDeviceMemoryProperties, commandPool, queue);

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

  VkDeviceSize indexBufferSize = sizeof(uint32_t) * model->getTotalMaterialIndexCount();

  std::vector<int> materialIndexList(model->getTotalMaterialIndexCount());
  for (int x = 0; x < model->getTotalMaterialIndexCount(); x++) {
    materialIndexList[x] = model->getTotalMaterialIndex(x);
  }

  VkBuffer indexStagingBuffer;
  VkDeviceMemory indexStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              indexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &indexStagingBuffer, 
                              &indexStagingBufferMemory);

  void* indexData;
  vkMapMemory(logicalDevice, indexStagingBufferMemory, 0, indexBufferSize, 0, &indexData);
  memcpy(indexData, materialIndexList.data(), indexBufferSize);
  vkUnmapMemory(logicalDevice, indexStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              indexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->materialIndexBufferMap[model], 
                              &this->materialIndexBufferMemoryMap[model]);

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, indexStagingBuffer, this->materialIndexBufferMap[model], indexBufferSize);
  
  vkDestroyBuffer(logicalDevice, indexStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, indexStagingBufferMemory, NULL);

  struct Material {
    alignas(16) float ambient[3];
    alignas(16) float diffuse[3];
    alignas(16) float specular[3];
    alignas(16) float emission[3];
  };

  VkDeviceSize materialBufferSize = sizeof(struct Material) * model->getMaterialCount();

  std::vector<Material> materialList(model->getMaterialCount());
  for (int x = 0; x < model->getMaterialCount(); x++) {
    memcpy(materialList[x].ambient, model->getMaterial(x).ambient, sizeof(float) * 3);
    memcpy(materialList[x].diffuse, model->getMaterial(x).diffuse, sizeof(float) * 3);
    memcpy(materialList[x].specular, model->getMaterial(x).specular, sizeof(float) * 3);
    memcpy(materialList[x].emission, model->getMaterial(x).emission, sizeof(float) * 3);
  }

  VkBuffer materialStagingBuffer;
  VkDeviceMemory materialStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              materialBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &materialStagingBuffer, 
                              &materialStagingBufferMemory);

  void* materialData;
  vkMapMemory(logicalDevice, materialStagingBufferMemory, 0, materialBufferSize, 0, &materialData);
  memcpy(materialData, materialList.data(), materialBufferSize);
  vkUnmapMemory(logicalDevice, materialStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              materialBufferSize,
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->materialBufferMap[model], 
                              &this->materialBufferMemoryMap[model]);

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, materialStagingBuffer, this->materialBufferMap[model], materialBufferSize);
  
  vkDestroyBuffer(logicalDevice, materialStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, materialStagingBufferMemory, NULL);

  struct LightContainer {
    alignas(4) int count;
    alignas(4) int indices[64];
  };

  LightContainer lightContainer = {
    .count = 0,
    .indices = {}
  };
  for (int x = 0; x < model->getTotalMaterialIndexCount(); x++) {
    float* materialEmission = model->getMaterial(model->getTotalMaterialIndex(x)).emission;
    if (materialEmission[0] > 0 || materialEmission[1] > 0 || materialEmission[2] > 0) {
      lightContainer.indices[lightContainer.count] = x;
      lightContainer.count += 1;
    }
  }
  
  VkBuffer materialLightStagingBuffer;
  VkDeviceMemory materialLightStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              sizeof(LightContainer), 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &materialLightStagingBuffer, 
                              &materialLightStagingBufferMemory);

  void* materialLightData;
  vkMapMemory(logicalDevice, materialLightStagingBufferMemory, 0, sizeof(LightContainer), 0, &materialLightData);
  memcpy(materialLightData, &lightContainer, sizeof(LightContainer));
  vkUnmapMemory(logicalDevice, materialLightStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              sizeof(LightContainer), 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->materialLightBufferMap[model], 
                              &this->materialLightBufferMemoryMap[model]);

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, materialLightStagingBuffer, this->materialLightBufferMap[model], sizeof(LightContainer));
  
  vkDestroyBuffer(logicalDevice, materialLightStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, materialLightStagingBufferMemory, NULL);
}