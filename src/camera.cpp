#include "camera.h"

Camera::Camera() {
  this->uniform = {
    .position = {
      0, 0, 0, 1
    },
    .right = {
      1, 0, 0, 0
    },
    .up = {
      0, 1, 0, 0
    },
    .forward = {
      0, 0, 1, 0
    },

    .frameCount = 0,
  };

  this->position[0] = 0;
  this->position[1] = 0;
  this->position[2] = 0;
  this->yaw = 0;
  this->pitch = 0;

  resetCursorPosition();
}

Camera::~Camera() {

}

float* Camera::getPosition() {
  return this->position;
}

void* Camera::getUniformPointer() {
  return &this->uniform;
}

uint32_t Camera::getUniformStructureSize() {
  return sizeof(CameraUniform);
}

void Camera::resetCursorPosition() {
  this->previousCursorPositionX = Input::getCursorPositionX();
  this->previousCursorPositionY = Input::getCursorPositionY();
}

void Camera::resetFrames() {
  this->uniform.frameCount = 0;
}

void Camera::update(bool isCursorActive) {
  static bool isCameraMoved = true;

  if (Input::checkKeyDown(GLFW_KEY_W)) {
    this->position[0] += cos(-this->yaw - (M_PI / 2)) * 0.1f;
    this->position[2] += sin(-this->yaw - (M_PI / 2)) * 0.1f;
  }
  if (Input::checkKeyDown(GLFW_KEY_S)) {
    this->position[0] -= cos(-this->yaw - (M_PI / 2)) * 0.1f;
    this->position[2] -= sin(-this->yaw - (M_PI / 2)) * 0.1f;
  }
  if (Input::checkKeyDown(GLFW_KEY_A)) {
    this->position[0] -= cos(-this->yaw) * 0.1f;
    this->position[2] -= sin(-this->yaw) * 0.1f;
  }
  if (Input::checkKeyDown(GLFW_KEY_D)) {
    this->position[0] += cos(-this->yaw) * 0.1f;
    this->position[2] += sin(-this->yaw) * 0.1f;
  }
  if (Input::checkKeyDown(GLFW_KEY_SPACE)) {
    this->position[1] += 0.1f;
  }
  if (Input::checkKeyDown(GLFW_KEY_LEFT_CONTROL)) {
    this->position[1] -= 0.1f;
  }

  static float previousPosition[3] = {this->position[0], this->position[1], this->position[2]};

  if (previousPosition[0] != this->position[0] || previousPosition[1] != this->position[1] || previousPosition[2] != this->position[2]) {
    isCameraMoved = true;

    previousPosition[0] = this->position[0];
    previousPosition[1] = this->position[1];
    previousPosition[2] = this->position[2];
  }

  if (isCursorActive) {
    double cursorPositionX = Input::getCursorPositionX();
    double cursorPositionY = Input::getCursorPositionY();

    if (this->previousCursorPositionX != cursorPositionX || this->previousCursorPositionY != cursorPositionY) {
      double mouseDifferenceX = this->previousCursorPositionX - cursorPositionX;
      double mouseDifferenceY = this->previousCursorPositionY - cursorPositionY;

      this->yaw += mouseDifferenceX * 0.0005f;

      this->previousCursorPositionX = cursorPositionX;
      this->previousCursorPositionY = cursorPositionY;

      isCameraMoved = true;
    }
  }

  if (isCameraMoved == true) {
    this->uniform.frameCount = 0;

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

    isCameraMoved = false;
  }
  else {
    this->uniform.frameCount += 1;
  }
}