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

std::string Model::getFileName() {
  return this->fileName;
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

uint32_t Model::getNormalCount() {
  return this->attrib.normals.size();
}

std::vector<float> Model::getNormals() {
  return this->attrib.normals;
}

uint32_t Model::getTextureCoordinateCount() {
  return this->attrib.texcoords.size();
}

std::vector<float> Model::getTextureCoordinates() {
  return this->attrib.texcoords;
};

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

uint32_t Model::getPrimitiveCount() {
  uint32_t primitiveCount = 0;
  for (int x = 0; x < this->shapes.size(); x++) {
    primitiveCount += this->shapes[x].mesh.num_face_vertices.size();
  }

  return primitiveCount;
}

uint32_t Model::getTransparentPrimitiveCount() {
  uint32_t primitiveCount = 0;
  for (int x = 0; x < this->shapes.size(); x++) {
    for (int y = 0; y < this->shapes[x].mesh.material_ids.size(); y++) {
      if (this->materials[this->shapes[x].mesh.material_ids[y]].dissolve < 1.0) {
        primitiveCount += 1;
      }
    }
  }

  return primitiveCount;
}

uint32_t Model::getTransparentPrimitiveOffset() {
  uint32_t primitiveCount = 0;
  for (int x = 0; x < this->shapes.size(); x++) {
    for (int y = 0; y < this->shapes[x].mesh.material_ids.size(); y++) {
      if (this->materials[this->shapes[x].mesh.material_ids[y]].dissolve < 1.0) {
        return primitiveCount;
      }

      primitiveCount += 1;
    }
  }

  return -1;
}