#pragma once

#include <stdint.h>

struct InstanceDescriptionContainer {
  uint32_t instanceCount; int padA[3];
  float transformMatrix[16];
};

class ModelInstance {
private:
public:
};