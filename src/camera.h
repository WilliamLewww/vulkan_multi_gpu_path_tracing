#pragma once

struct Camera {
  float position[4];
  float right[4];
  float up[4];
  float forward[4];

  uint32_t frameCount;
};