#pragma once

#include <vulkan/vulkan.h>
#include <string.h>

#include "matrix.h"

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