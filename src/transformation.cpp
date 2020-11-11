#include "transformation.h"

Transformation::Transformation() {
  float identityMatrix[16] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
  };

  memcpy(this->transformMatrix, identityMatrix, sizeof(float) * 16);
}

Transformation::~Transformation() {

}

float* Transformation::getTransformMatrix() {
  return this->transformMatrix;
}

VkTransformMatrixKHR Transformation::getVulkanTransformMatrix() {
  VkTransformMatrixKHR vulkanTransformMatrix = {};
  memcpy(vulkanTransformMatrix.matrix, this->transformMatrix, sizeof(float) * 12);

  return vulkanTransformMatrix;
}