#pragma once

#include <vulkan/vulkan.h>
#include <string.h>

#include "matrix.h"

class Transformation {
private:
  Matrix4x4 transformationMatrix;

  float position[3];
  float scale[3];
public:
  Transformation();
  Transformation(float* position, float* scale);
  ~Transformation();

  float* getPosition();
  float* getScale();

  Matrix4x4 getTransformMatrix();
  VkTransformMatrixKHR getVulkanTransformMatrix();
};