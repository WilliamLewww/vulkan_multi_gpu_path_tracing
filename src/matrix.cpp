#include "matrix.h"

float& Matrix4x4::operator[](int x) {
  return this->data[x];
}

Matrix4x4 createIdentityMatrix4x4() {
  Matrix4x4 matrix = {
    .data = {
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
    }
  };

  return matrix;
}

Matrix4x4 createTranslateMatrix4x4(float x, float y, float z) {
  Matrix4x4 matrix = {
    .data = {
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      x, y, z, 1
    }
  };

  return matrix;
}

Matrix4x4 createScaleMatrix4x4(float x, float y, float z) {
  Matrix4x4 matrix = {
    .data = {
      x, 0, 0, 0,
      0, y, 0, 0,
      0, 0, z, 0,
      0, 0, 0, 1
    }
  };

  return matrix;
}