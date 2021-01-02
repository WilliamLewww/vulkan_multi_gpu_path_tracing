#pragma once

#include <stdint.h>
#include <math.h>

#include "input.h"

class Camera {
private:
  struct CameraUniform {
    alignas(16) float position[4];
    alignas(16) float right[4];
    alignas(16) float up[4];
    alignas(16) float forward[4];

    alignas(4)  uint32_t frameCount;
  };

  CameraUniform uniform;

  float position[3];
  float yaw;
  float pitch;

  double previousCursorPositionX;
  double previousCursorPositionY;
public:
  Camera();
  ~Camera();

  float* getPosition();

  void* getUniformPointer();
  uint32_t getUniformStructureSize();

  void resetCursorPosition();

  void update();
  void updateOnlyFrames();
};