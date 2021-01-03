#include "transformation.h"

Transformation::Transformation() {
  Matrix4x4 transformationMatrix = createIdentityMatrix4x4();
  memcpy(&this->transformationMatrix, &transformationMatrix, sizeof(Matrix4x4));
}

Transformation::Transformation(float* position) {
  this->position[0] = position[0];
  this->position[1] = position[1];
  this->position[2] = position[2];

  Matrix4x4 transformationMatrix = createIdentityMatrix4x4();
  transformationMatrix = multiplyMatrix4x4(createTranslateMatrix4x4(position[0], position[1], position[2]), transformationMatrix);
  memcpy(&this->transformationMatrix, &transformationMatrix, sizeof(Matrix4x4));
}

Transformation::~Transformation() {

}

Matrix4x4 Transformation::getTransformMatrix() {
  return this->transformationMatrix;
}

VkTransformMatrixKHR Transformation::getVulkanTransformMatrix() {
  // row major
  VkTransformMatrixKHR vulkanTransformMatrix = {
    .matrix = {
      {this->transformationMatrix[0], this->transformationMatrix[4], this->transformationMatrix[8], this->transformationMatrix[12]},
      {this->transformationMatrix[1], this->transformationMatrix[5], this->transformationMatrix[9], this->transformationMatrix[13]},
      {this->transformationMatrix[2], this->transformationMatrix[6], this->transformationMatrix[10], this->transformationMatrix[14]}
    }
  };

  return vulkanTransformMatrix;
}