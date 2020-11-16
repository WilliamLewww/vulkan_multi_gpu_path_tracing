#include "instance_manager.h"

InstanceManager::InstanceManager() {

}

InstanceManager::~InstanceManager() {

}

ModelInstance InstanceManager::getInstance(Device* device, uint32_t index) {
  return this->modelInstanceMap[device][index];
}

std::vector<ModelInstance> InstanceManager::getInstanceList(Device* device) {
  return this->modelInstanceMap[device];
}

std::vector<float> InstanceManager::getTotalTransformBuffer(Device* device) {
  std::vector<ModelInstance>& modelInstanceList = this->modelInstanceMap[device];

  std::vector<float> transformBuffer(modelInstanceList.size() * 16);
  for (int x = 0; x < modelInstanceList.size(); x++) {
    memcpy((16 * x) + transformBuffer.data(), modelInstanceList[x].transformation.getTransformMatrix(), sizeof(float) * 16);
  }

  return transformBuffer;
}

uint32_t InstanceManager::getInstanceCount(Device* device) {
  return this->modelInstanceMap[device].size();
}

void InstanceManager::initializeContainerOnDevice(Device* device) {
  this->modelInstanceMap.insert(std::pair<Device*, std::vector<ModelInstance>>(device, std::vector<ModelInstance>()));
}

void InstanceManager::addInstance(Device* device, Model* model, uint32_t modelIndex, uint32_t instanceIndex, float* transformationMatrix) {
  Transformation transformation;

  if (transformationMatrix == NULL) {
    transformation = Transformation();
  }
  else {
    transformation = Transformation(transformationMatrix);
  }

  ModelInstance modelInstance = {
    .modelIndex = modelIndex,
    .instanceIndex = instanceIndex,
    .transformation = transformation,

    .vertexBuffer = model->getVertexBuffer(device),
    .indexBuffer = model->getIndexBuffer(device),

    .primitiveCount = model->getPrimitiveCount(),
    .vertexCount = model->getVertexCount()
  };

  this->modelInstanceMap[device].push_back(modelInstance);
}