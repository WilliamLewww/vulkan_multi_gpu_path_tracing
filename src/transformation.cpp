#include "transformation.h"

float& Matrix4x4::operator[](int x) {
  return this->data[x];
}

Transformation::Transformation() {
  // column major
  Matrix4x4 transformationMatrix = {
    .data = {
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0
    }
  };

  memcpy(&this->transformationMatrix, &transformationMatrix, sizeof(Matrix4x4));
}

Transformation::Transformation(Matrix4x4 transformationMatrix) {
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