#define STB_IMAGE_IMPLEMENTATION

#include "model_instance_set.h"

ModelInstanceSet::ModelInstanceSet(std::map<Model*, std::vector<TRS>> modelFrequencyMap, 
                                                 VkDevice logicalDevice, 
                                                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                 VkCommandPool commandPool,
                                                 VkQueue queue) {

  int modelIndex = 0;
  int instanceIndex = 0;
  std::vector<float> totalVertexList;
  std::vector<float> totalNormalList;
  std::vector<float> totalTextureCoordinateList;
  std::vector<uint32_t> totalIndexList;
  std::vector<uint32_t> totalNormalIndexList;
  std::vector<uint32_t> totalTextureCoordinateIndexList;
  std::vector<uint32_t> totalMaterialIndexList;
  std::vector<Material> totalMaterialList;
  LightContainer lightContainer = {
    .count = 0,
    .indicesPrimitive = {},
    .indicesInstance = {}
  };
  std::vector<std::string> totalTextureNameList;

  uint32_t cumulativeVertexOffset = 0;
  uint32_t cumulativeNormalOffset = 0;
  uint32_t cumulativeTextureCoordinateOffset = 0;
  uint32_t cumulativeIndexOffset = 0;
  uint32_t cumulativeMaterialIndexOffset = 0;
  uint32_t cumulativeMaterialOffset = 0;

  for (std::pair<Model*, std::vector<TRS>> pair : modelFrequencyMap) {
    this->createVertexBuffer(pair.first, logicalDevice, physicalDeviceMemoryProperties, commandPool, queue, &totalVertexList, &totalNormalList, &totalTextureCoordinateList);
    this->createIndexBuffer(pair.first, logicalDevice, physicalDeviceMemoryProperties, commandPool, queue, &totalIndexList, &totalNormalIndexList, &totalTextureCoordinateIndexList);
    this->createMaterialBuffers(pair.first, logicalDevice, physicalDeviceMemoryProperties, commandPool, queue, &totalMaterialIndexList, &totalMaterialList, &totalTextureNameList, &lightContainer);

    for (int x = 0; x < pair.second.size(); x++) {
      this->modelInstanceList.push_back(new ModelInstance(pair.first, &this->vertexBufferMap[pair.first], &this->indexBufferMap[pair.first], modelIndex, instanceIndex, pair.second[x].position, pair.second[x].scale));
      this->modelInstanceMap[pair.first].push_back(this->modelInstanceList.back());

      this->vertexOffsetList.push_back(cumulativeVertexOffset);
      this->normalOffsetList.push_back(cumulativeNormalOffset);
      this->textureCoordinateOffsetList.push_back(cumulativeTextureCoordinateOffset);
      this->indexOffsetList.push_back(cumulativeIndexOffset);
      this->materialIndexOffsetList.push_back(cumulativeMaterialIndexOffset);
      this->materialOffsetList.push_back(cumulativeMaterialOffset);

      if (x == pair.second.size() - 1) {
        cumulativeVertexOffset += pair.first->getVertexCount();
        cumulativeNormalOffset += pair.first->getNormalCount();
        cumulativeTextureCoordinateOffset += pair.first->getTextureCoordinateCount();
        cumulativeIndexOffset += pair.first->getTotalIndexCount();
        cumulativeMaterialIndexOffset += pair.first->getTotalMaterialIndexCount();
        cumulativeMaterialOffset += pair.first->getMaterialCount();
      }

      for (int y = 0; y < pair.first->getTotalMaterialIndexCount(); y++) {
        float* materialEmission = pair.first->getMaterial(pair.first->getTotalMaterialIndex(y)).emission;
        if (materialEmission[0] > 0 || materialEmission[1] > 0 || materialEmission[2] > 0) {
          lightContainer.indicesPrimitive[lightContainer.count] = y;
          lightContainer.indicesInstance[lightContainer.count] = instanceIndex;
          lightContainer.count += 1;
        }
      }

      instanceIndex += 1;
    }

    modelIndex += 1;
  }

  uint32_t instanceCount = this->modelInstanceList.size();
  std::vector<float> totalTransformList = this->getTotalTransformList();

  this->instanceUniform = {
    .buffer = {}
  };
  memcpy((float*)&this->instanceUniform, &instanceCount, sizeof(uint32_t));
  for (int x = 0; x < this->vertexOffsetList.size(); x++) {
    memcpy(4 + (x * 4) + (float*)&this->instanceUniform, &this->vertexOffsetList[x], sizeof(uint32_t));
    memcpy(36 + (x * 4) + (float*)&this->instanceUniform, &this->normalOffsetList[x], sizeof(uint32_t));
    memcpy(68 + (x * 4) + (float*)&this->instanceUniform, &this->textureCoordinateOffsetList[x], sizeof(uint32_t));

    memcpy(100 + (x * 4) + (float*)&this->instanceUniform, &this->indexOffsetList[x], sizeof(uint32_t));
    memcpy(132 + (x * 4) + (float*)&this->instanceUniform, &this->materialIndexOffsetList[x], sizeof(uint32_t));
    memcpy(164 + (x * 4) + (float*)&this->instanceUniform, &this->materialOffsetList[x], sizeof(uint32_t));
  }
  memcpy(196 + (float*)&this->instanceUniform, totalTransformList.data(), sizeof(float) * totalTransformList.size());

  createTotalBuffers(totalVertexList,
                     totalNormalList,
                     totalTextureCoordinateList,
                     totalIndexList,
                     totalNormalIndexList,
                     totalTextureCoordinateIndexList,
                     totalMaterialIndexList,
                     totalMaterialList,
                     lightContainer,
                     logicalDevice, 
                     physicalDeviceMemoryProperties, 
                     commandPool,
                     queue);

  this->descriptorTotalVertexBufferInfo = {
    .buffer = this->totalVertexBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->descriptorTotalNormalBufferInfo = {
    .buffer = this->totalNormalBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->descriptorTotalIndexBufferInfo = {
    .buffer = this->totalIndexBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->descriptorTotalNormalIndexBufferInfo = {
    .buffer = this->totalNormalIndexBuffer,
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

  this->descriptorTotalTextureCoordinateBufferInfo = {
    .buffer = this->totalTextureCoordinateBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->descriptorTotalTextureCoordinateIndexBufferInfo = {
    .buffer = this->totalTextureCoordinateIndexBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->materialImageList.resize(totalTextureNameList.size());
  this->materialImageViewList.resize(totalTextureNameList.size());
  this->materialImageMemory.resize(totalTextureNameList.size());
  this->descriptorMaterialImageInfoList.resize(totalTextureNameList.size());

  for (int x = 0; x < totalTextureNameList.size(); x++) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(std::string("res/" + totalTextureNameList[x]).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
      printf("Failed to load texture!\n");
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    BufferFactory::createBuffer(logicalDevice,
                                physicalDeviceMemoryProperties,
                                imageSize, 
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                                &stagingBuffer, 
                                &stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    stbi_image_free(pixels);

    ImageFactory::createImage(logicalDevice,
                              physicalDeviceMemoryProperties,
                              texWidth, 
                              texHeight, 
                              VK_FORMAT_R8G8B8A8_SRGB, 
                              VK_IMAGE_TILING_OPTIMAL, 
                              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->materialImageList[x], 
                              &this->materialImageMemory[x]);

    ImageFactory::transitionImageLayout(logicalDevice,
                                        commandPool,
                                        queue,
                                        this->materialImageList[x], 
                                        VK_FORMAT_R8G8B8A8_SRGB, 
                                        VK_IMAGE_LAYOUT_UNDEFINED, 
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    ImageFactory::copyBufferToImage(logicalDevice,
                                    commandPool,
                                    queue,
                                    stagingBuffer, this->materialImageList[x], (uint32_t)texWidth, (uint32_t)texHeight);

    ImageFactory::transitionImageLayout(logicalDevice,
                                        commandPool,
                                        queue,
                                        this->materialImageList[x], 
                                        VK_FORMAT_R8G8B8A8_SRGB, 
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    ImageFactory::createImageView(logicalDevice, this->materialImageList[x], VK_FORMAT_R8G8B8A8_SRGB, &this->materialImageViewList[x]);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);

    this->descriptorMaterialImageInfoList[x] = {
      .imageView = this->materialImageViewList[x],
      .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };
  }
}

ModelInstanceSet::~ModelInstanceSet() {
  for (int x = 0; x < this->modelInstanceList.size(); x++) {
    delete this->modelInstanceList[x];
  }
}

void ModelInstanceSet::createVertexBuffer(Model* model,
                                          VkDevice logicalDevice, 
                                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                          VkCommandPool commandPool,
                                          VkQueue queue,
                                          std::vector<float>* totalVertexList,
                                          std::vector<float>* totalNormalList,
                                          std::vector<float>* totalTextureCoordinateList) {

  VkDeviceSize bufferSize = sizeof(float) * model->getVertexCount();

  std::vector<float> vertexList = model->getVertices();
  std::vector<float> normalList = model->getNormals();
  std::vector<float> textureCoordinateList = model->getTextureCoordinates();
  std::copy(vertexList.begin(), vertexList.end(), std::back_inserter(*totalVertexList));
  std::copy(normalList.begin(), normalList.end(), std::back_inserter(*totalNormalList));
  std::copy(textureCoordinateList.begin(), textureCoordinateList.end(), std::back_inserter(*totalTextureCoordinateList));
  
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

void ModelInstanceSet::createIndexBuffer(Model* model,
                                         VkDevice logicalDevice, 
                                         VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                         VkCommandPool commandPool,
                                         VkQueue queue,
                                         std::vector<uint32_t>* totalIndexList,
                                         std::vector<uint32_t>* totalNormalIndexList,
                                         std::vector<uint32_t>* totalTextureCoordinateIndexList) {

  VkDeviceSize bufferSize = sizeof(uint32_t) * model->getTotalIndexCount();

  std::vector<uint32_t> positionIndexList(model->getTotalIndexCount());
  for (int x = 0; x < model->getTotalIndexCount(); x++) {
    positionIndexList[x] = model->getTotalIndex(x).vertex_index;
    totalIndexList->push_back(model->getTotalIndex(x).vertex_index);
    totalNormalIndexList->push_back(model->getTotalIndex(x).normal_index);
    totalTextureCoordinateIndexList->push_back(model->getTotalIndex(x).texcoord_index);
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

void ModelInstanceSet::createMaterialBuffers(Model* model,
                                                    VkDevice logicalDevice, 
                                                    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                    VkCommandPool commandPool,
                                                    VkQueue queue,
                                                    std::vector<uint32_t>* totalMaterialIndexList,
                                                    std::vector<Material>* totalMaterialList,
                                                    std::vector<std::string>* totalTextureNameList,
                                                    LightContainer* lightContainer) {

  for (int x = 0; x < model->getTotalMaterialIndexCount(); x++) {
    totalMaterialIndexList->push_back(model->getTotalMaterialIndex(x));
  }

  std::vector<Material> materialList(model->getMaterialCount());
  for (int x = 0; x < model->getMaterialCount(); x++) {
    Material material = {
      .ambient = {},
      .diffuse = {},
      .specular = {},
      .emission = {},

      .shininess = 0,
      .dissolve = 0,
      .ior = 0,

      .diffuseTextureIndex = -1
    };
    memcpy(material.ambient, model->getMaterial(x).ambient, sizeof(float) * 3);
    memcpy(material.diffuse, model->getMaterial(x).diffuse, sizeof(float) * 3);
    memcpy(material.specular, model->getMaterial(x).specular, sizeof(float) * 3);
    memcpy(material.emission, model->getMaterial(x).emission, sizeof(float) * 3);

    material.shininess = model->getMaterial(x).shininess;
    material.dissolve = model->getMaterial(x).dissolve;
    material.ior = model->getMaterial(x).ior;

    if (!model->getMaterial(x).diffuse_texname.empty()) {
      int textureIndex = totalTextureNameList->size();
      for (int y = 0; y < totalTextureNameList->size(); y++) {
        if ((*totalTextureNameList)[y] == model->getMaterial(x).diffuse_texname) {
          textureIndex = y;
        }
      }
      material.diffuseTextureIndex = textureIndex;
      if (textureIndex == totalTextureNameList->size()) {
        totalTextureNameList->push_back(model->getMaterial(x).diffuse_texname);
      }
    }

    if (model->getMaterial(x).unknown_parameter.size() > 0) {
      material.type = std::stoi(model->getMaterial(x).unknown_parameter["type"]);
    }
    else {
      material.type = 0;
    }

    totalMaterialList->push_back(material);
  }
}

void ModelInstanceSet::createTotalBuffers(std::vector<float> totalVertexList,
                                          std::vector<float> totalNormalList,
                                          std::vector<float> totalTextureCoordinateList,
                                          std::vector<uint32_t> totalIndexList,
                                          std::vector<uint32_t> totalNormalIndexList,
                                          std::vector<uint32_t> totalTextureCoordinateIndexList,
                                          std::vector<uint32_t> totalMaterialIndexList,
                                          std::vector<Material> totalMaterialList,
                                          LightContainer lightContainer,
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

  VkDeviceSize totalNormalBufferSize = sizeof(float) * totalNormalList.size();
  
  VkBuffer totalNormalStagingBuffer;
  VkDeviceMemory totalNormalStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              totalNormalBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &totalNormalStagingBuffer, 
                              &totalNormalStagingBufferMemory);

  void* totalNormalData;
  vkMapMemory(logicalDevice, totalNormalStagingBufferMemory, 0, totalNormalBufferSize, 0, &totalNormalData);
  memcpy(totalNormalData, totalNormalList.data(), totalNormalBufferSize);
  vkUnmapMemory(logicalDevice, totalNormalStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              totalNormalBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->totalNormalBuffer, 
                              &this->totalNormalBufferMemory);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, totalNormalStagingBuffer, this->totalNormalBuffer, totalNormalBufferSize);

  vkDestroyBuffer(logicalDevice, totalNormalStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, totalNormalStagingBufferMemory, NULL);

  VkDeviceSize totalTextureCoordinateBufferSize = sizeof(float) * totalTextureCoordinateList.size();
  
  VkBuffer totalTextureCoordinateStagingBuffer;
  VkDeviceMemory totalTextureCoordinateStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              totalTextureCoordinateBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &totalTextureCoordinateStagingBuffer, 
                              &totalTextureCoordinateStagingBufferMemory);

  void* totalTextureCoordinateData;
  vkMapMemory(logicalDevice, totalTextureCoordinateStagingBufferMemory, 0, totalTextureCoordinateBufferSize, 0, &totalTextureCoordinateData);
  memcpy(totalTextureCoordinateData, totalTextureCoordinateList.data(), totalTextureCoordinateBufferSize);
  vkUnmapMemory(logicalDevice, totalTextureCoordinateStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              totalTextureCoordinateBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->totalTextureCoordinateBuffer, 
                              &this->totalTextureCoordinateBufferMemory);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, totalTextureCoordinateStagingBuffer, this->totalTextureCoordinateBuffer, totalTextureCoordinateBufferSize);

  vkDestroyBuffer(logicalDevice, totalTextureCoordinateStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, totalTextureCoordinateStagingBufferMemory, NULL);

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

  VkDeviceSize totalNormalIndexBufferSize = sizeof(uint32_t) * totalNormalIndexList.size();
  
  VkBuffer totalNormalIndexStagingBuffer;
  VkDeviceMemory totalNormalIndexStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              totalNormalIndexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &totalNormalIndexStagingBuffer, 
                              &totalNormalIndexStagingBufferMemory);

  void* totalNormalIndexData;
  vkMapMemory(logicalDevice, totalNormalIndexStagingBufferMemory, 0, totalNormalIndexBufferSize, 0, &totalNormalIndexData);
  memcpy(totalNormalIndexData, totalNormalIndexList.data(), totalNormalIndexBufferSize);
  vkUnmapMemory(logicalDevice, totalNormalIndexStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              totalNormalIndexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->totalNormalIndexBuffer, 
                              &this->totalNormalIndexBufferMemory);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, totalNormalIndexStagingBuffer, this->totalNormalIndexBuffer, totalNormalIndexBufferSize);

  vkDestroyBuffer(logicalDevice, totalNormalIndexStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, totalNormalIndexStagingBufferMemory, NULL);

  VkDeviceSize totalTextureCoordinateIndexBufferSize = sizeof(uint32_t) * totalTextureCoordinateIndexList.size();
  
  VkBuffer totalTextureCoordinateIndexStagingBuffer;
  VkDeviceMemory totalTextureCoordinateIndexStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              totalTextureCoordinateIndexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &totalTextureCoordinateIndexStagingBuffer, 
                              &totalTextureCoordinateIndexStagingBufferMemory);

  void* totalTextureCoordinateIndexData;
  vkMapMemory(logicalDevice, totalTextureCoordinateIndexStagingBufferMemory, 0, totalTextureCoordinateIndexBufferSize, 0, &totalTextureCoordinateIndexData);
  memcpy(totalTextureCoordinateIndexData, totalTextureCoordinateIndexList.data(), totalTextureCoordinateIndexBufferSize);
  vkUnmapMemory(logicalDevice, totalTextureCoordinateIndexStagingBufferMemory);

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              totalTextureCoordinateIndexBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->totalTextureCoordinateIndexBuffer, 
                              &this->totalTextureCoordinateIndexBufferMemory);  

  BufferFactory::copyBuffer(logicalDevice, commandPool, queue, totalTextureCoordinateIndexStagingBuffer, this->totalTextureCoordinateIndexBuffer, totalTextureCoordinateIndexBufferSize);

  vkDestroyBuffer(logicalDevice, totalTextureCoordinateIndexStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, totalTextureCoordinateIndexStagingBufferMemory, NULL);

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

  VkDeviceSize totalMaterialLightBufferSize = sizeof(LightContainer);
  
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
  memcpy(totalMaterialLightData, &lightContainer, totalMaterialLightBufferSize);
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

std::vector<float> ModelInstanceSet::getTotalTransformList() {
  std::vector<float> transformList(this->modelInstanceList.size() * 16);
  for (int x = 0; x < this->modelInstanceList.size(); x++) {
    memcpy((16 * x) + transformList.data(), this->modelInstanceList[x]->getTransformation().getTransformMatrix().data, sizeof(float) * 16);
  }

  return transformList;
}

void* ModelInstanceSet::getUniformBufferPointer() {
  return &this->instanceUniform;
}

uint32_t ModelInstanceSet::getUniformBufferSize() {
  return sizeof(InstanceUniform);
}

uint32_t ModelInstanceSet::getModelInstanceIndex(Model* model) {
  for (int x = 0; x < this->modelInstanceList.size(); x++) {
    if (this->modelInstanceList[x]->getModel() == model) {
      return x;
    }
  }

  return -1;
}

uint32_t ModelInstanceSet::getInstanceCount() {
  return this->modelInstanceList.size();
}

ModelInstance* ModelInstanceSet::getModelInstance(int index) {
  return this->modelInstanceList[index];
}

std::map<Model*, std::vector<ModelInstance*>> ModelInstanceSet::getModelInstanceMap() {
  return this->modelInstanceMap;
}

VkDescriptorBufferInfo* ModelInstanceSet::getDescriptorTotalVertexBufferInfoPointer() {
  return &this->descriptorTotalVertexBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceSet::getDescriptorTotalNormalBufferInfoPointer() {
  return &this->descriptorTotalNormalBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceSet::getDescriptorTotalIndexBufferInfoPointer() {
  return &this->descriptorTotalIndexBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceSet::getDescriptorTotalNormalIndexBufferInfoPointer() {
  return &this->descriptorTotalNormalIndexBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceSet::getDescriptorTotalMaterialIndexBufferInfoPointer() {
  return &this->descriptorTotalMaterialIndexBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceSet::getDescriptorTotalMaterialBufferInfoPointer() {
  return &this->descriptorTotalMaterialBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceSet::getDescriptorTotalMaterialLightBufferInfoPointer() {
  return &this->descriptorTotalMaterialLightBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceSet::getDescriptorTotalTextureCoordinateBufferInfoPointer() {
  return &this->descriptorTotalTextureCoordinateBufferInfo;
}

VkDescriptorBufferInfo* ModelInstanceSet::getDescriptorTotalTextureCoordinateIndexBufferInfoPointer() {
  return &this->descriptorTotalTextureCoordinateIndexBufferInfo;
}

VkDescriptorImageInfo* ModelInstanceSet::getDescriptorMaterialImageInfoListPointer() {
  return this->descriptorMaterialImageInfoList.data();
}

std::vector<ModelInstance*> ModelInstanceSet::getModelInstanceList() {
  return this->modelInstanceList;
}

void ModelInstanceSet::updateUniformBuffer() {
  uint32_t instanceCount = this->modelInstanceList.size();
  std::vector<float> totalTransformList = this->getTotalTransformList();

  memcpy((float*)&this->instanceUniform, &instanceCount, sizeof(uint32_t));
  for (int x = 0; x < this->vertexOffsetList.size(); x++) {
    memcpy(4 + (x * 4) + (float*)&this->instanceUniform, &this->vertexOffsetList[x], sizeof(uint32_t));
    memcpy(36 + (x * 4) + (float*)&this->instanceUniform, &this->normalOffsetList[x], sizeof(uint32_t));
    memcpy(68 + (x * 4) + (float*)&this->instanceUniform, &this->textureCoordinateOffsetList[x], sizeof(uint32_t));

    memcpy(100 + (x * 4) + (float*)&this->instanceUniform, &this->indexOffsetList[x], sizeof(uint32_t));
    memcpy(132 + (x * 4) + (float*)&this->instanceUniform, &this->materialIndexOffsetList[x], sizeof(uint32_t));
    memcpy(164 + (x * 4) + (float*)&this->instanceUniform, &this->materialOffsetList[x], sizeof(uint32_t));
  }
  memcpy(196 + (float*)&this->instanceUniform, totalTransformList.data(), sizeof(float) * totalTransformList.size());
}