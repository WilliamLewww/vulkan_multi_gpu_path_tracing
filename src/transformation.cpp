#include "transformation.h"

Transformation::Transformation() {
  // column major
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
  // row major
  VkTransformMatrixKHR vulkanTransformMatrix = {
    .matrix = {
      {this->transformMatrix[0], this->transformMatrix[4], this->transformMatrix[8], this->transformMatrix[12]},
      {this->transformMatrix[1], this->transformMatrix[5], this->transformMatrix[9], this->transformMatrix[13]},
      {this->transformMatrix[2], this->transformMatrix[6], this->transformMatrix[10], this->transformMatrix[14]}
    }
  };

  return vulkanTransformMatrix;
}