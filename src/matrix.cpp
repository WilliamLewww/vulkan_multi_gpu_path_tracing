#include "matrix.h"

float& Matrix4x4::operator[](int x) {
  return this->data[x];
}