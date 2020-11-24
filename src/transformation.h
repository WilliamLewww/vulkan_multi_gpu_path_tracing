#pragma once

#include <vulkan/vulkan.h>

#include "string.h"

struct Matrix4x4 {
  float data[16];

  float& operator[](int index);
};

class Transformation {
private:
  Matrix4x4 transformationMatrix;
public:
  Transformation();
  Transformation(Matrix4x4 transformationMatrix);
  ~Transformation();

  Matrix4x4 getTransformMatrix();
  VkTransformMatrixKHR getVulkanTransformMatrix();
};