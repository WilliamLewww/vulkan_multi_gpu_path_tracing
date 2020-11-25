#pragma once

#include "model.h"
#include "transformation.h"

class ModelInstance {
private:
  Model* model;

  VkBuffer* vertexBuffer;
  VkBuffer* indexBuffer;

  uint32_t modelIndex;
  uint32_t instanceIndex;
  Transformation transformation;
public:
  ModelInstance(Model* model, VkBuffer* vertexBuffer, VkBuffer* indexBuffer, uint32_t modelIndex, uint32_t instanceIndex, Matrix4x4 transformationMatrix);
  ~ModelInstance();

  Model* getModel();

  uint32_t getInstanceIndex();

  Transformation getTransformation();

  VkBuffer getVertexBuffer();
  VkBuffer getIndexBuffer();
};