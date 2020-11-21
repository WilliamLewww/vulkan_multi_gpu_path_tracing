#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#endif

#include "model.h"

Model::Model(std::string fileName) {
  chdir("res");

  this->fileName = fileName;
  this->success = tinyobj::LoadObj(&this->attrib, &this->shapes, &this->materials, &this->warning, &this->error, fileName.c_str());

  chdir("..");

  this->checkError();
}

Model::~Model() {

}

bool Model::checkError() {
  if (!this->warning.empty()) {
    printf("%s\n", this->warning.c_str());
  }

  if (!this->error.empty()) {
    printf("%s\n", this->error.c_str());
  }

  return this->success;
}

uint32_t Model::getVertexCount() {
  return this->attrib.vertices.size();
}

std::vector<float> Model::getVertices() {
  return this->attrib.vertices;
}

uint32_t Model::getTotalIndexCount() {
  uint32_t indexCount = 0;
  for (int x = 0; x < this->shapes.size(); x++) {
    indexCount += this->shapes[x].mesh.indices.size();
  }

  return indexCount;
}

tinyobj::index_t Model::getTotalIndex(uint32_t index) {
  for (int x = 0; x < this->shapes.size(); x++) {
    if (index >= this->shapes[x].mesh.indices.size()) {
      index -= this->shapes[x].mesh.indices.size();
    }
    else {
      return this->shapes[x].mesh.indices[index];
    }
  }

  return {-1, -1, -1};
}

uint32_t Model::getTotalMaterialIndexCount() {
  uint32_t indexCount = 0;
  for (int x = 0; x < this->shapes.size(); x++) {
    indexCount += this->shapes[x].mesh.material_ids.size();
  }

  return indexCount;
}

int Model::getTotalMaterialIndex(uint32_t index) {
  for (int x = 0; x < this->shapes.size(); x++) {
    if (index >= this->shapes[x].mesh.material_ids.size()) {
      index -= this->shapes[x].mesh.material_ids.size();
    }
    else {
      return this->shapes[x].mesh.material_ids[index];
    }
  }

  return -1;
}

uint32_t Model::getMaterialCount() {
  return this->materials.size();
}

tinyobj::material_t Model::getMaterial(uint32_t index) {
  return this->materials[index];
}