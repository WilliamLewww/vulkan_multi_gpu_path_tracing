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