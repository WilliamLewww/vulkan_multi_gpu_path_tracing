#include "camera.h"

Camera::Camera() {
  this->uniform = {
    .position = {
      0, 0, 0, 1
    },
    .right = {
      1, 0, 0, 1
    },
    .up = {
      0, 1, 0, 1
    },
    .forward = {
      0, 0, 1, 1
    },

    .frameCount = 0,
  };

  this->position[0] = 0;
  this->position[1] = 0;
  this->position[2] = 0;
  this->yaw = 0;
  this->pitch = 0;
}

Camera::~Camera() {

}

CameraUniform Camera::getUniform() {
  return this->uniform;
}