#include "model_instance.h"

ModelInstance::ModelInstance(Model* model, uint32_t modelIndex, uint32_t instanceIndex) {
  this->model = model;
  
  this->modelIndex = modelIndex;
  this->instanceIndex = instanceIndex;

  this->transformation = Transformation();
}

ModelInstance::~ModelInstance() {

}