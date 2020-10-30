#pragma once

#include <stdint.h>

struct CameraUniform {
  float position[4];
  float right[4];
  float up[4];
  float forward[4];

  uint32_t frameCount;
};

class Camera {
private:
  CameraUniform uniform;

  float position[3];
  float yaw;
  float pitch;
public:
  Camera();
  ~Camera();

  CameraUniform getUniform();
};