#include "model_instance.h"

ModelInstance::ModelInstance(Model* model, VkBuffer* vertexBuffer, VkBuffer* indexBuffer, uint32_t modelIndex, uint32_t instanceIndex) {
  this->model = model;

  this->vertexBuffer = vertexBuffer;
  this->indexBuffer = indexBuffer;
  
  this->modelIndex = modelIndex;
  this->instanceIndex = instanceIndex;

  this->transformation = Transformation();
}

ModelInstance::~ModelInstance() {

}

Transformation ModelInstance::getTransformation() {
  return this->transformation;
}

VkBuffer ModelInstance::getVertexBuffer() {
  return *this->vertexBuffer;
}

VkBuffer ModelInstance::getIndexBuffer() {
  return *this->indexBuffer;
}