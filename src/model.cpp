#include "model.h"

Model::Model(std::string fileName) {
  chdir("res");

  this->fileName = fileName;
  this->success = tinyobj::LoadObj(&this->attrib, &this->shapes, &this->materials, &this->warning, &this->error, fileName.c_str());

  chdir("..");

  this->checkError();
}

Model::~Model() {

}

bool Model::checkError() {
  if (!this->warning.empty()) {
    printf("%s\n", this->warning.c_str());
  }

  if (!this->error.empty()) {
    printf("%s\n", this->error.c_str());
  }

  return this->success;
}

uint32_t Model::getVertexCount() {
  return this->attrib.vertices.size();
}

std::vector<float> Model::getVertices() {
  return this->attrib.vertices;
}

uint32_t Model::getTotalIndexCount() {
  uint32_t indexCount = 0;
  for (int x = 0; x < this->shapes.size(); x++) {
    indexCount += this->shapes[x].mesh.indices.size();
  }

  return indexCount;
}

tinyobj::index_t Model::getTotalIndex(uint32_t index) {
  for (int x = 0; x < this->shapes.size(); x++) {
    if (index >= this->shapes[x].mesh.indices.size()) {
      index -= this->shapes[x].mesh.indices.size();
    }
    else {
      return this->shapes[x].mesh.indices[index];
    }
  }

  return {-1, -1, -1};
}

uint32_t Model::getTotalMaterialIndexCount() {
  uint32_t indexCount = 0;
  for (int x = 0; x < this->shapes.size(); x++) {
    indexCount += this->shapes[x].mesh.material_ids.size();
  }

  return indexCount;
}

int Model::getTotalMaterialIndex(uint32_t index) {
  for (int x = 0; x < this->shapes.size(); x++) {
    if (index >= this->shapes[x].mesh.material_ids.size()) {
      index -= this->shapes[x].mesh.material_ids.size();
    }
    else {
      return this->shapes[x].mesh.material_ids[index];
    }
  }

  return -1;
}

uint32_t Model::getMaterialCount() {
  return this->materials.size();
}

tinyobj::material_t Model::getMaterial(uint32_t index) {
  return this->materials[index];
}

uint32_t Model::getPrimitiveCount() {
  uint32_t primitiveCount = 0;
  for (int x = 0; x < this->shapes.size(); x++) {
    primitiveCount += this->shapes[x].mesh.num_face_vertices.size();
  }

  return primitiveCount;
}

VkBuffer Model::getVertexBuffer(Device* device) {
  return this->deviceMap[device].vertexBuffer;
}

VkBuffer Model::getIndexBuffer(Device* device) {
  return this->deviceMap[device].indexBuffer;
}

VkBuffer Model::getMaterialIndexBuffer(Device* device) {
  return this->deviceMap[device].materialIndexBuffer;
}

VkBuffer Model::getMaterialBuffer(Device* device) {
  return this->deviceMap[device].materialBuffer;
}

VkBuffer Model::getMaterialLightBuffer(Device* device) {
  return this->deviceMap[device].materialLightBuffer;
}

void Model::createVertexBuffer(Device* device) {
  VkDeviceSize positionBufferSize = sizeof(float) * this->getVertexCount();
  
  VkBuffer positionStagingBuffer;
  VkDeviceMemory positionStagingBufferMemory;
  Buffer::createBuffer(device, positionBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &positionStagingBuffer, &positionStagingBufferMemory);

  void* positionData;
  vkMapMemory(device->getLogicalDevice(), positionStagingBufferMemory, 0, positionBufferSize, 0, &positionData);
  memcpy(positionData, this->getVertices().data(), positionBufferSize);
  vkUnmapMemory(device->getLogicalDevice(), positionStagingBufferMemory);

  Buffer::createBuffer(device, positionBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->deviceMap[device].vertexBuffer, &this->deviceMap[device].vertexBufferMemory);  

  Buffer::copyBuffer(device, positionStagingBuffer, this->deviceMap[device].vertexBuffer, positionBufferSize);

  vkDestroyBuffer(device->getLogicalDevice(), positionStagingBuffer, NULL);
  vkFreeMemory(device->getLogicalDevice(), positionStagingBufferMemory, NULL);
}

void Model::createIndexBuffer(Device* device) {
  VkDeviceSize bufferSize = sizeof(uint32_t) * this->getTotalIndexCount();

  std::vector<uint32_t> positionIndexList(this->getTotalIndexCount());
  for (int x = 0; x < this->getTotalIndexCount(); x++) {
    positionIndexList[x] = this->getTotalIndex(x).vertex_index;
  }
  
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  Buffer::createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

  void* data;
  vkMapMemory(device->getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, positionIndexList.data(), bufferSize);
  vkUnmapMemory(device->getLogicalDevice(), stagingBufferMemory);

  Buffer::createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->deviceMap[device].indexBuffer, &this->deviceMap[device].indexBufferMemory);

  Buffer::copyBuffer(device, stagingBuffer, this->deviceMap[device].indexBuffer, bufferSize);
  
  vkDestroyBuffer(device->getLogicalDevice(), stagingBuffer, NULL);
  vkFreeMemory(device->getLogicalDevice(), stagingBufferMemory, NULL);
}

void Model::createMaterialBuffers(Device* device) {
VkDeviceSize indexBufferSize = sizeof(uint32_t) * this->getTotalMaterialIndexCount();

  std::vector<int> materialIndexList(this->getTotalMaterialIndexCount());
  for (int x = 0; x < this->getTotalMaterialIndexCount(); x++) {
    materialIndexList[x] = this->getTotalMaterialIndex(x);
  }

  VkBuffer indexStagingBuffer;
  VkDeviceMemory indexStagingBufferMemory;
  Buffer::createBuffer(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &indexStagingBuffer, &indexStagingBufferMemory);

  void* indexData;
  vkMapMemory(device->getLogicalDevice(), indexStagingBufferMemory, 0, indexBufferSize, 0, &indexData);
  memcpy(indexData, materialIndexList.data(), indexBufferSize);
  vkUnmapMemory(device->getLogicalDevice(), indexStagingBufferMemory);

  Buffer::createBuffer(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->deviceMap[device].materialIndexBuffer, &this->deviceMap[device].materialIndexBufferMemory);

  Buffer::copyBuffer(device, indexStagingBuffer, this->deviceMap[device].materialIndexBuffer, indexBufferSize);
  
  vkDestroyBuffer(device->getLogicalDevice(), indexStagingBuffer, NULL);
  vkFreeMemory(device->getLogicalDevice(), indexStagingBufferMemory, NULL);

  VkDeviceSize materialBufferSize = sizeof(struct Material) * this->getMaterialCount();

  std::vector<Material> materialList(this->getMaterialCount());
  for (int x = 0; x < this->getMaterialCount(); x++) {
    memcpy(materialList[x].ambient, this->getMaterial(x).ambient, sizeof(float) * 3);
    memcpy(materialList[x].diffuse, this->getMaterial(x).diffuse, sizeof(float) * 3);
    memcpy(materialList[x].specular, this->getMaterial(x).specular, sizeof(float) * 3);
    memcpy(materialList[x].emission, this->getMaterial(x).emission, sizeof(float) * 3);
  }

  VkBuffer materialStagingBuffer;
  VkDeviceMemory materialStagingBufferMemory;
  Buffer::createBuffer(device, materialBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &materialStagingBuffer, &materialStagingBufferMemory);

  void* materialData;
  vkMapMemory(device->getLogicalDevice(), materialStagingBufferMemory, 0, materialBufferSize, 0, &materialData);
  memcpy(materialData, materialList.data(), materialBufferSize);
  vkUnmapMemory(device->getLogicalDevice(), materialStagingBufferMemory);

  Buffer::createBuffer(device, materialBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->deviceMap[device].materialBuffer, &this->deviceMap[device].materialBufferMemory);

  Buffer::copyBuffer(device, materialStagingBuffer, this->deviceMap[device].materialBuffer, materialBufferSize);
  
  vkDestroyBuffer(device->getLogicalDevice(), materialStagingBuffer, NULL);
  vkFreeMemory(device->getLogicalDevice(), materialStagingBufferMemory, NULL);

  LightContainer lightContainer = {};
  for (int x = 0; x < this->getTotalMaterialIndexCount(); x++) {
    float* materialEmission = this->getMaterial(this->getTotalMaterialIndex(x)).emission;
    if (materialEmission[0] > 0 || materialEmission[1] > 0 || materialEmission[2] > 0) {
      lightContainer.indices[lightContainer.count] = x;
      lightContainer.count += 1;
    }
  }
  
  VkBuffer materialLightStagingBuffer;
  VkDeviceMemory materialLightStagingBufferMemory;
  Buffer::createBuffer(device, sizeof(LightContainer), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &materialLightStagingBuffer, &materialLightStagingBufferMemory);

  void* materialLightData;
  vkMapMemory(device->getLogicalDevice(), materialLightStagingBufferMemory, 0, sizeof(LightContainer), 0, &materialLightData);
  memcpy(materialLightData, &lightContainer, sizeof(LightContainer));
  vkUnmapMemory(device->getLogicalDevice(), materialLightStagingBufferMemory);

  Buffer::createBuffer(device, sizeof(LightContainer), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->deviceMap[device].materialLightBuffer, &this->deviceMap[device].materialLightBufferMemory);

  Buffer::copyBuffer(device, materialLightStagingBuffer, this->deviceMap[device].materialLightBuffer, sizeof(LightContainer));
  
  vkDestroyBuffer(device->getLogicalDevice(), materialLightStagingBuffer, NULL);
  vkFreeMemory(device->getLogicalDevice(), materialLightStagingBufferMemory, NULL);
}

void Model::initializeOnDevice(Device* device) {
  this->createVertexBuffer(device);
  this->createIndexBuffer(device);
  this->createMaterialBuffers(device);
}