#pragma once

#include <stdint.h>
#include <math.h>

#include "input.h"

class Camera {
private:
  struct CameraUniform {
    float position[4];
    float right[4];
    float up[4];
    float forward[4];

    uint32_t frameCount;
  };

  CameraUniform uniform;

  float position[3];
  float yaw;
  float pitch;
public:
  Camera();
  ~Camera();

  void* getUniformPointer();
  uint32_t getUniformStructureSize();

  void update();
};