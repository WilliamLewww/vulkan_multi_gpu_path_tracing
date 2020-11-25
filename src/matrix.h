#pragma once

struct Matrix4x4 {
  float data[16];

  float& operator[](int index);
};

Matrix4x4 createIdentityMatrix4x4();