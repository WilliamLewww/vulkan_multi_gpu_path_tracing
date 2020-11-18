#include "instance_manager.h"

InstanceManager::InstanceManager() {

}

InstanceManager::~InstanceManager() {

}

ModelInstance InstanceManager::getInstance(Device* device, uint32_t index) {
  return this->deviceMap[device].modelInstanceList[index];
}

std::vector<ModelInstance> InstanceManager::getInstanceList(Device* device) {
  return this->deviceMap[device].modelInstanceList;
}

std::vector<float> InstanceManager::getTotalTransformBuffer(Device* device) {
  std::vector<ModelInstance>& modelInstanceList = this->deviceMap[device].modelInstanceList;

  std::vector<float> transformBuffer(modelInstanceList.size() * 16);
  for (int x = 0; x < modelInstanceList.size(); x++) {
    memcpy((16 * x) + transformBuffer.data(), modelInstanceList[x].transformation.getTransformMatrix(), sizeof(float) * 16);
  }

  return transformBuffer;
}

uint32_t InstanceManager::getInstanceCount(Device* device) {
  return this->deviceMap[device].modelInstanceList.size();
}

void InstanceManager::initializeContainerOnDevice(Device* device) {
  this->deviceMap.insert(std::pair<Device*, DeviceContainer>(device, DeviceContainer()));
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

  this->deviceMap[device].modelInstanceList.push_back(modelInstance);
}

void InstanceManager::print() {
  printf("==========Instance Manager==========\n");
  for (std::pair<Device*, DeviceContainer> pair : this->deviceMap) {
    printf("  Device: %p\n", pair.first);
    for (int x = 0; x < pair.second.modelInstanceList.size(); x++) {
      float* transformationMatrix = pair.second.modelInstanceList[x].transformation.getTransformMatrix();

      std::string transformationString = "        [";
      for (int x = 0; x < 16; x++) {
        if (x != 0 && x % 4 == 0) {
          transformationString += "\n        [";
        }
        transformationString += std::to_string(transformationMatrix[x]);
        if ((x + 1) % 4 == 0) {
          transformationString += "]";
        }
        else { 
          transformationString += ", ";
        }
      }
      transformationString += "\n";

      printf("    Instance Index: %d\n", pair.second.modelInstanceList[x].instanceIndex);
      printf("      Model Index: %d\n", pair.second.modelInstanceList[x].modelIndex);
      printf("      Primitive Count: %d\n", pair.second.modelInstanceList[x].primitiveCount);
      printf("      Transformation:\n");
      printf("%s", transformationString.c_str());
    }
  }
}