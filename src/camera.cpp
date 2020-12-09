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

void* Camera::getUniformPointer() {
  return &this->uniform;
}

uint32_t Camera::getUniformStructureSize() {
  return sizeof(CameraUniform);
}

void Camera::update() {
  int isCameraMoved = 0;

  if (Input::checkKeyDown(GLFW_KEY_W)) {
    this->position[0] += cos(-this->yaw - (M_PI / 2)) * 0.1f;
    this->position[2] += sin(-this->yaw - (M_PI / 2)) * 0.1f;
    isCameraMoved = 1;
  }
  if (Input::checkKeyDown(GLFW_KEY_S)) {
    this->position[0] -= cos(-this->yaw - (M_PI / 2)) * 0.1f;
    this->position[2] -= sin(-this->yaw - (M_PI / 2)) * 0.1f;
    isCameraMoved = 1;
  }
  if (Input::checkKeyDown(GLFW_KEY_A)) {
    this->position[0] -= cos(-this->yaw) * 0.1f;
    this->position[2] -= sin(-this->yaw) * 0.1f;
    isCameraMoved = 1;
  }
  if (Input::checkKeyDown(GLFW_KEY_D)) {
    this->position[0] += cos(-this->yaw) * 0.1f;
    this->position[2] += sin(-this->yaw) * 0.1f;
    isCameraMoved = 1;
  }
  if (Input::checkKeyDown(GLFW_KEY_SPACE)) {
    this->position[1] += 0.1f;
    isCameraMoved = 1;
  }
  if (Input::checkKeyDown(GLFW_KEY_LEFT_CONTROL)) {
    this->position[1] -= 0.1f;
    isCameraMoved = 1;
  }

  static double previousCursorPositionX = Input::getCursorPositionX();
  static double previousCursorPositionY = Input::getCursorPositionY();

  double cursorPositionX = Input::getCursorPositionX();
  double cursorPositionY = Input::getCursorPositionY();

  if (previousCursorPositionX != cursorPositionX || previousCursorPositionY != cursorPositionY) {
    double mouseDifferenceX = previousCursorPositionX - cursorPositionX;
    double mouseDifferenceY = previousCursorPositionY - cursorPositionY;

    this->yaw += mouseDifferenceX * 0.0005f;

    previousCursorPositionX = cursorPositionX;
    previousCursorPositionY = cursorPositionY;

    isCameraMoved = 1;
  }

  this->uniform.position[0] = this->position[0]; 
  this->uniform.position[1] = this->position[1]; 
  this->uniform.position[2] = this->position[2]; 
  this->uniform.position[3] = 1.0;

  this->uniform.forward[0] = cosf(this->pitch) * cosf(-this->yaw - (M_PI / 2.0));
  this->uniform.forward[1] = sinf(this->pitch);
  this->uniform.forward[2] = cosf(this->pitch) * sinf(-this->yaw - (M_PI / 2.0));
  this->uniform.forward[3] = 0.0f;

  this->uniform.right[0] = this->uniform.forward[1] * this->uniform.up[2] - this->uniform.forward[2] * this->uniform.up[1];
  this->uniform.right[1] = this->uniform.forward[2] * this->uniform.up[0] - this->uniform.forward[0] * this->uniform.up[2];
  this->uniform.right[2] = this->uniform.forward[0] * this->uniform.up[1] - this->uniform.forward[1] * this->uniform.up[0];
  this->uniform.right[3] = 0.0f;

  if (isCameraMoved == 1) {
    this->uniform.frameCount = 0;
  }
  else {
    this->uniform.frameCount += 1;
  }
}