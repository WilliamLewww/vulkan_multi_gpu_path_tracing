#pragma once

#include <vulkan/vulkan.h>
#include <string.h>

#include "matrix.h"

class Transformation {
private:
  Matrix4x4 transformationMatrix;

  float position[3];
public:
  Transformation();
  Transformation(float* position);
  ~Transformation();

  Matrix4x4 getTransformMatrix();
  VkTransformMatrixKHR getVulkanTransformMatrix();
};