#include "matrix.h"

float& Matrix4x4::operator[](int x) {
  return this->data[x];
}

TRS createTRS(std::vector<float> position, std::vector<float> scale) {
  TRS trs;
  
  if (position.size() == 3) {
    trs.position[0] = position[0];
    trs.position[1] = position[1];
    trs.position[2] = position[2];
  }
  else {
    trs.position[0] = 0;
    trs.position[1] = 0;
    trs.position[2] = 0;
  }

  if (scale.size() == 3) {
    trs.scale[0] = scale[0];
    trs.scale[1] = scale[1];
    trs.scale[2] = scale[2];
  }
  else {
    trs.scale[0] = 1;
    trs.scale[1] = 1;
    trs.scale[2] = 1;
  }

  return trs;
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

Matrix4x4 multiplyMatrix4x4(Matrix4x4 a, Matrix4x4 b) {
  Matrix4x4 matrix;
  matrix[0] = (a[0] * b[0]) + (a[1] * b[4]) + (a[2] * b[8]) + (a[3] * b[12]);
  matrix[1] = (a[0] * b[1]) + (a[1] * b[5]) + (a[2] * b[9]) + (a[3] * b[13]);
  matrix[2] = (a[0] * b[2]) + (a[1] * b[6]) + (a[2] * b[10]) + (a[3] * b[14]);
  matrix[3] = (a[0] * b[3]) + (a[1] * b[7]) + (a[2] * b[11]) + (a[3] * b[15]);
  matrix[4] = (a[4] * b[0]) + (a[5] * b[4]) + (a[6] * b[8]) + (a[7] * b[12]);
  matrix[5] = (a[4] * b[1]) + (a[5] * b[5]) + (a[6] * b[9]) + (a[7] * b[13]);
  matrix[6] = (a[4] * b[2]) + (a[5] * b[6]) + (a[6] * b[10]) + (a[7] * b[14]);
  matrix[7] = (a[4] * b[3]) + (a[5] * b[7]) + (a[6] * b[11]) + (a[7] * b[15]);
  matrix[8] = (a[8] * b[0]) + (a[9] * b[4]) + (a[10] * b[8]) + (a[11] * b[12]);
  matrix[9] = (a[8] * b[1]) + (a[9] * b[5]) + (a[10] * b[9]) + (a[11] * b[13]);
  matrix[10] = (a[8] * b[2]) + (a[9] * b[6]) + (a[10] * b[10]) + (a[11] * b[14]);
  matrix[11] = (a[8] * b[3]) + (a[9] * b[7]) + (a[10] * b[11]) + (a[11] * b[15]);
  matrix[12] = (a[12] * b[0]) + (a[13] * b[4]) + (a[14] * b[8]) + (a[15] * b[12]);
  matrix[13] = (a[12] * b[1]) + (a[13] * b[5]) + (a[14] * b[9]) + (a[15] * b[13]);
  matrix[14] = (a[12] * b[2]) + (a[13] * b[6]) + (a[14] * b[10]) + (a[15] * b[14]);
  matrix[15] = (a[12] * b[3]) + (a[13] * b[7]) + (a[14] * b[11]) + (a[15] * b[15]);

  return matrix;
}