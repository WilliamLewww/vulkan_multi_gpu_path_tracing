#include "camera.h"

Camera::Camera(float positionX, float positionY, float positionZ, float pitch, float yaw) {
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

    .pitch = 0,
    .yaw = 0,
    .frameCount = 0,

    .waveLength = 380
  };

  this->position[0] = positionX;
  this->position[1] = positionY;
  this->position[2] = positionZ;
  this->yaw = yaw;
  this->pitch = pitch;

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

  this->uniform.pitch = this->pitch;
  this->uniform.yaw = this->yaw;
  this->uniform.frameCount = 0;

  this->uniform.waveLength = 380;

  this->isCameraMoved = true;
  this->previousPosition[0] = this->position[0]; 
  this->previousPosition[1] = this->position[1]; 
  this->previousPosition[2] = this->position[2];
  this->previousPitch = this->pitch;
  this->previousYaw = this->yaw;

  resetCursorPosition();
}

Camera::~Camera() {

}

float* Camera::getPosition() {
  return this->position;
}

float* Camera::getYaw() {
  return &this->yaw;
}

float* Camera::getPitch() {
  return &this->pitch;
}

float* Camera::getWaveLength() {
  return &this->uniform.waveLength;
}

uint32_t* Camera::getFrameCount() {
  return &this->uniform.frameCount;
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

void Camera::update(bool isKeyboardActive, bool isCursorActive, bool isInverseYaw) {
  if (isKeyboardActive) {
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
  }

  if (this->previousPosition[0] != this->position[0] || this->previousPosition[1] != this->position[1] || this->previousPosition[2] != this->position[2]) {
    this->isCameraMoved = true;

    this->previousPosition[0] = this->position[0];
    this->previousPosition[1] = this->position[1];
    this->previousPosition[2] = this->position[2];
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
    }
  }

  if (this->previousYaw != this->yaw || this->previousPitch != this->pitch) {
    this->previousYaw = this->yaw;
    this->previousPitch = this->pitch;

    this->isCameraMoved = true;
  }

  if (this->isCameraMoved == true) {
    this->uniform.position[0] = this->position[0]; 
    this->uniform.position[1] = this->position[1]; 
    this->uniform.position[2] = this->position[2]; 
    this->uniform.position[3] = 1.0;

    if (isInverseYaw) {
      this->uniform.forward[0] = cosf(this->pitch) * cosf(this->yaw - (M_PI / 2.0));
      this->uniform.forward[1] = sinf(this->pitch);
      this->uniform.forward[2] = cosf(this->pitch) * sinf(this->yaw - (M_PI / 2.0));
    }
    else {
      this->uniform.forward[0] = cosf(this->pitch) * cosf(-this->yaw - (M_PI / 2.0));
      this->uniform.forward[1] = sinf(this->pitch);
      this->uniform.forward[2] = cosf(this->pitch) * sinf(-this->yaw - (M_PI / 2.0));
    }
    this->uniform.forward[3] = 0.0f;

    this->uniform.right[0] = this->uniform.forward[1] * this->uniform.up[2] - this->uniform.forward[2] * this->uniform.up[1];
    this->uniform.right[1] = this->uniform.forward[2] * this->uniform.up[0] - this->uniform.forward[0] * this->uniform.up[2];
    this->uniform.right[2] = this->uniform.forward[0] * this->uniform.up[1] - this->uniform.forward[1] * this->uniform.up[0];
    this->uniform.right[3] = 0.0f;

    this->uniform.pitch = this->pitch;
    this->uniform.yaw = this->yaw;
    this->uniform.frameCount = 0;

    this->isCameraMoved = false;
  }
  else {
    this->uniform.frameCount += 1;
    this->uniform.waveLength = (rand() % (780 - 380 + 1) + 380) + ((rand() % 1000) / 1000.0);
  }
}