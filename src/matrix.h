#pragma once

struct Matrix4x4 {
  float data[16];

  float& operator[](int index);
};

Matrix4x4 createIdentityMatrix4x4();
Matrix4x4 createTranslateMatrix4x4(float x, float y, float z);
Matrix4x4 createScaleMatrix4x4(float x, float y, float z);

Matrix4x4 multiplyMatrix4x4(Matrix4x4 a, Matrix4x4 b);