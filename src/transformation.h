#pragma once

#include <vulkan/vulkan.h>

#include "string.h"

class Transformation {
private:
  float transformMatrix[16];
public:
  Transformation();
  Transformation(float matrix[16]);
  ~Transformation();

  float* getTransformMatrix();
  VkTransformMatrixKHR getVulkanTransformMatrix();
};