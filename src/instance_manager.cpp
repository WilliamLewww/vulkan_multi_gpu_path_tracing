#include "instance_manager.h"

InstanceManager::InstanceManager() {

}

InstanceManager::~InstanceManager() {

}

ModelInstance InstanceManager::getInstance(Device* device, uint32_t index) {
  return this->modelInstanceMap[device][index];
}

void InstanceManager::initializeContainerOnDevice(Device* device) {
  this->modelInstanceMap.insert(std::pair<Device*, std::vector<ModelInstance>>(device, std::vector<ModelInstance>()));
}

void InstanceManager::addInstance(Device* device, Model* model, uint32_t modelIndex, uint32_t instanceIndex) {
  ModelInstance modelInstance = {
    .modelIndex = modelIndex,
    .instanceIndex = instanceIndex,
    .transformation = Transformation(),

    .vertexBuffer = model->getVertexBuffer(device),
    .indexBuffer = model->getIndexBuffer(device),

    .primitiveCount = model->getPrimitiveCount(),
    .vertexCount = model->getVertexCount()
  };

  this->modelInstanceMap[device].push_back(modelInstance);
}