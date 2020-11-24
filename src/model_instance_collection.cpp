#include "model_instance_collection.h"

ModelInstanceCollection::ModelInstanceCollection(std::map<Model*, uint32_t> modelFrequencyMap, 
                                                 VkDevice logicalDevice, 
                                                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                 VkCommandPool commandPool,
                                                 VkQueue queue) {

  int modelIndex = 0;
  int instanceIndex = 0;
  std::vector<float> totalVertexList;
  std::vector<uint32_t> totalIndexList;
  std::vector<uint32_t> totalMaterialIndexList;
  std::vector<Material> totalMaterialList;
  std::vector<LightContainer> totalMaterialLightList;

  std::pair<Model*, uint32_t> previousPair;

  for (std::pair<Model*, uint32_t> pair : modelFrequencyMap) {
    this->createVertexBuffer(pair.first, logicalDevice, physicalDeviceMemoryProperties, commandPool, queue, &totalVertexList);
    this->createIndexBuffer(pair.first, logicalDevice, physicalDeviceMemoryProperties, commandPool, queue, &totalIndexList);
    this->createMaterialBuffers(pair.first, logicalDevice, physicalDeviceMemoryProperties, commandPool, queue, &totalMaterialIndexList, &totalMaterialList, &totalMaterialLightList);

    for (int x = 0; x < pair.second; x++) {
      this->modelInstanceList.push_back(new ModelInstance(pair.first, &this->vertexBufferMap[pair.first], &this->indexBufferMap[pair.first], modelIndex, instanceIndex));
      this->modelInstanceMap[pair.first].push_back(this->modelInstanceList.back());

      if (modelIndex == 0) {
        this->vertexOffsetList.push_back(0);
        this->indexOffsetList.push_back(0);
      }
      else {
        this->vertexOffsetList.push_back(previousPair.first->getVertexCount());
        this->indexOffsetList.push_back(previousPair.first->getTotalIndexCount());
      }
    }
    previousPair = pair;
    modelIndex += 1;
  }

  uint32_t instanceCount = this->modelInstanceList.size();
  std::vector<float> totalTransformList = this->getTotalTransformList();

  this->uniformBuffer = (float*)malloc(512);
  memcpy(this->uniformBuffer, &instanceCount, sizeof(uint32_t));
  memcpy(4 + this->uniformBuffer, this->vertexOffsetList.data(), sizeof(uint32_t) * this->vertexOffsetList.size());
  memcpy(36 + this->uniformBuffer, this->indexOffsetList.data(), sizeof(uint32_t) * this->indexOffsetList.size());
  memcpy(68 + this->uniformBuffer, totalTransformList.data(), sizeof(float) * totalTransformList.size());

  createTotalBuffers(totalVertexList,
                     totalIndexList,
                     totalMaterialIndexList,
                     totalMaterialList,
                     totalMaterialLightList,
                     logicalDevice, 
                     physicalDeviceMemoryProperties, 
                     commandPool,
                     queue);

  this->descriptorTotalVertexBufferInfo = {
    .buffer = this->totalVertexBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->descriptorTotalIndexBufferInfo = {
    .buffer = this->totalIndexBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->descriptorTotalMaterialIndexBufferInfo = {
    .buffer = this->totalMaterialIndexBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->descriptorTotalMaterialBufferInfo = {
    .buffer = this->totalMaterialBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->descriptorTotalMaterialLightBufferInfo = {
    .buffer = this->totalMaterialLightBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
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
                                                 VkQueue queue,
                                                 std::vector<float>* totalVertexList) {

  VkDeviceSize bufferSize = sizeof(float) * model->getVertexCount();

  std::vector<float> vertexList = model->getVertices();
  std::copy(vertexList.begin(), vertexList.end(), std::back_inserter(*totalVertexList));
  
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
  memcpy(data, vertexList.data(), bufferSize);
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
                                                VkQueue queue,
                                                std::vector<uint32_t>* totalIndexList) {

  VkDeviceSize bufferSize = sizeof(uint32_t) * model->getTotalIndexCount();

  std::vector<uint32_t> positionIndexList(model->getTotalIndexCount());
  for (int x = 0; x < model->getTotalIndexCount(); x++) {
    positionIndexList[x] = model->getTotalIndex(x).vertex_index;
    totalIndexList->push_back(positionIndexList[x]);
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
                                                    VkQueue queue,
                                                    std::vector<uint32_t>* totalMaterialIndexList,
                                                    std::vector<Material>* totalMaterialList,
                                                    std::vector<LightContainer>* totalMaterialLightList) {

  VkDeviceSize indexBufferSize = sizeof(uint32_t) * model->getTotalMaterialIndexCount();

  std::vector<uint32_t> materialIndexList(model->getTotalMaterialIndexCount());
  for (int x = 0; x < model->getTotalMaterialIndexCount(); x++) {
    materialIndexList[x] = model->getTotalMaterialIndex(x);
    totalMaterialIndexList->push_back(materialIndexList[x]);
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

  VkDeviceSize materialBufferSize = sizeof(struct Material) * model->getMaterialCount();

  std::vector<Material> materialList(model->getMaterialCount());
  for (int x = 0; x < model->getMaterialCount(); x++) {
    memcpy(materialList[x].ambient, model->getMaterial(x).ambient, sizeof(float) * 3);
    memcpy(materialList[x].diffuse, model->getMaterial(x).diffuse, sizeof(float) * 3);
    memcpy(materialList[x].specular, model->getMaterial(x).specular, sizeof(float) * 3);
    memcpy(materialList[x].emission, model->getMaterial(x).emission, sizeof(float) * 3);

    totalMaterialList->push_back(materialList[x]);
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
  totalMaterialLightList->push_back(lightContainer);
  
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

void ModelInstanceCollection::createTotalBuffers(std::vector<float> totalVertexList,
                                                 std::vector<uint32_t> totalIndexList,
                                                 std::vector<uint32_t> totalMaterialIndexList,
                                                 std::vector<Material> totalMaterialList,
                                                 std::vector<LightContainer> totalMaterialLightList,
                                                 VkDevice logicalDevice, 
                                                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                 VkCommandPool commandPool,
                                                 VkQueue queue) {

  VkDeviceSize totalVertexBufferSize = sizeof(float) * totalVertexList.size();
  
  VkBuffer totalVertexStagingBuffer;
  VkDeviceMemory totalVertexStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              totalVertexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &totalVertexStagingBuffer, 
                              &totalVertexStagingBufferMemory);

  void* totalVertexData;
  vkMapMemory(logicalDevice, totalVertexStagingBufferMemory, 0, totalVertexBufferSize, 0, &totalVertexData);
  memcpy(totalVertexData, totalVertexList.data(), totalVertexBufferSize);
  vkUnmapMemory(logicalDevice, totalVertexStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              totalVertexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->totalVertexBuffer, 
                              &this->totalVertexBufferMemory);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, totalVertexStagingBuffer, this->totalVertexBuffer, totalVertexBufferSize);

  vkDestroyBuffer(logicalDevice, totalVertexStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, totalVertexStagingBufferMemory, NULL);

  VkDeviceSize totalIndexBufferSize = sizeof(uint32_t) * totalIndexList.size();
  
  VkBuffer totalIndexStagingBuffer;
  VkDeviceMemory totalIndexStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              totalIndexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &totalIndexStagingBuffer, 
                              &totalIndexStagingBufferMemory);

  void* totalIndexData;
  vkMapMemory(logicalDevice, totalIndexStagingBufferMemory, 0, totalIndexBufferSize, 0, &totalIndexData);
  memcpy(totalIndexData, totalIndexList.data(), totalIndexBufferSize);
  vkUnmapMemory(logicalDevice, totalIndexStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              totalIndexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->totalIndexBuffer, 
                              &this->totalIndexBufferMemory);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, totalIndexStagingBuffer, this->totalIndexBuffer, totalIndexBufferSize);

  vkDestroyBuffer(logicalDevice, totalIndexStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, totalIndexStagingBufferMemory, NULL);

  VkDeviceSize totalMaterialIndexBufferSize = sizeof(uint32_t) * totalMaterialIndexList.size();
  
  VkBuffer totalMaterialIndexStagingBuffer;
  VkDeviceMemory totalMaterialIndexStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              totalMaterialIndexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &totalMaterialIndexStagingBuffer, 
                              &totalMaterialIndexStagingBufferMemory);

  void* totalMaterialIndexData;
  vkMapMemory(logicalDevice, totalMaterialIndexStagingBufferMemory, 0, totalMaterialIndexBufferSize, 0, &totalMaterialIndexData);
  memcpy(totalMaterialIndexData, totalMaterialIndexList.data(), totalMaterialIndexBufferSize);
  vkUnmapMemory(logicalDevice, totalMaterialIndexStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              totalMaterialIndexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->totalMaterialIndexBuffer, 
                              &this->totalMaterialIndexBufferMemory);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, totalMaterialIndexStagingBuffer, this->totalMaterialIndexBuffer, totalMaterialIndexBufferSize);

  vkDestroyBuffer(logicalDevice, totalMaterialIndexStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, totalMaterialIndexStagingBufferMemory, NULL);

  VkDeviceSize totalMaterialBufferSize = sizeof(Material) * totalMaterialList.size();
  
  VkBuffer totalMaterialStagingBuffer;
  VkDeviceMemory totalMaterialStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              totalMaterialBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &totalMaterialStagingBuffer, 
                              &totalMaterialStagingBufferMemory);

  void* totalMaterialData;
  vkMapMemory(logicalDevice, totalMaterialStagingBufferMemory, 0, totalMaterialBufferSize, 0, &totalMaterialData);
  memcpy(totalMaterialData, totalMaterialList.data(), totalMaterialBufferSize);
  vkUnmapMemory(logicalDevice, totalMaterialStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              totalMaterialBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->totalMaterialBuffer, 
                              &this->totalMaterialBufferMemory);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, totalMaterialStagingBuffer, this->totalMaterialBuffer, totalMaterialBufferSize);

  vkDestroyBuffer(logicalDevice, totalMaterialStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, totalMaterialStagingBufferMemory, NULL);

  VkDeviceSize totalMaterialLightBufferSize = sizeof(LightContainer) * totalMaterialLightList.size();
  
  VkBuffer totalMaterialLightStagingBuffer;
  VkDeviceMemory totalMaterialLightStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              totalMaterialLightBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &totalMaterialLightStagingBuffer, 
                              &totalMaterialLightStagingBufferMemory);

  void* totalMaterialLightData;
  vkMapMemory(logicalDevice, totalMaterialLightStagingBufferMemory, 0, totalMaterialLightBufferSize, 0, &totalMaterialLightData);
  memcpy(totalMaterialLightData, totalMaterialLightList.data(), totalMaterialLightBufferSize);
  vkUnmapMemory(logicalDevice, totalMaterialLightStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              totalMaterialLightBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->totalMaterialLightBuffer, 
                              &this->totalMaterialLightBufferMemory);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, totalMaterialLightStagingBuffer, this->totalMaterialLightBuffer, totalMaterialLightBufferSize);

  vkDestroyBuffer(logicalDevice, totalMaterialLightStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, totalMaterialLightStagingBufferMemory, NULL);
}

std::vector<float> ModelInstanceCollection::getTotalTransformList() {
  std::vector<float> transformList(this->modelInstanceList.size() * 16);
  for (int x = 0; x < this->modelInstanceList.size(); x++) {
    memcpy((16 * x) + transformList.data(), this->modelInstanceList[x]->getTransformation().getTransformMatrix().data, sizeof(float) * 16);
  }

  return transformList;
}

void* ModelInstanceCollection::getUniformBufferPointer() {
  return this->uniformBuffer;
}

uint32_t ModelInstanceCollection::getUniformBufferSize() {
  return 512;
}

std::map<Model*, std::vector<ModelInstance*>> ModelInstanceCollection::getModelInstanceMap() {
  return this->modelInstanceMap;
}

VkDescriptorBufferInfo* ModelInstanceCollection::getDescriptorTotalVertexBufferInfoPointer() {
  return &this->descriptorTotalVertexBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceCollection::getDescriptorTotalIndexBufferInfoPointer() {
  return &this->descriptorTotalIndexBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceCollection::getDescriptorTotalMaterialIndexBufferInfoPointer() {
  return &this->descriptorTotalMaterialIndexBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceCollection::getDescriptorTotalMaterialBufferInfoPointer() {
  return &this->descriptorTotalMaterialBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceCollection::getDescriptorTotalMaterialLightBufferInfoPointer() {
  return &this->descriptorTotalMaterialLightBufferInfo;
}

std::vector<ModelInstance*> ModelInstanceCollection::getModelInstanceList() {
  return this->modelInstanceList;
}