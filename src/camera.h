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

    alignas(4)  float pitch;
    alignas(4)  float yaw;
    alignas(4)  uint32_t frameCount;

    alignas(4)  float waveLength;
  };

  CameraUniform uniform;

  float position[3];
  float yaw;
  float pitch;

  double previousCursorPositionX;
  double previousCursorPositionY;

  bool isWaveLengthPaused;

  bool isCameraMoved;
  float previousPosition[3];
  float previousYaw;
  float previousPitch;
public:
  Camera(float positionX = 0.0, float positionY = 0.0, float positionZ = 0.0, float pitch = 0.0, float yaw = 0.0);
  ~Camera();

  float* getPosition();
  float* getYaw();
  float* getPitch();
  float* getWaveLength();
  uint32_t* getFrameCount();

  bool* getIsWaveLengthPaused();

  void* getUniformPointer();
  uint32_t getUniformStructureSize();

  void resetCursorPosition();
  void resetFrames();

  void update(bool isKeyboardActive = true, bool isCursorActive = true, bool isInverseYaw = false);
};