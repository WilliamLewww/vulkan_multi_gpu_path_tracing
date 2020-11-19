#pragma once

#include "model.h"
#include "transformation.h"

class ModelInstance {
private:
  Model* model;

  uint32_t modelIndex;
  uint32_t instanceIndex;
  Transformation transformation;
public:
  ModelInstance(Model* model, uint32_t modelIndex, uint32_t instanceIndex);
  ~ModelInstance();
};