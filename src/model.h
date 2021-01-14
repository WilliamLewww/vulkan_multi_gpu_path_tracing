#pragma once

#include <unistd.h>

#include "tiny_obj_loader/tiny_obj_loader.h"

class Model {
private:
  std::string fileName;
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warning;
  std::string error;
  bool success;
public:
  Model(std::string fileName);
  ~Model();

  std::string getFileName();

  bool checkError();

  uint32_t getVertexCount();
  std::vector<float> getVertices();

  uint32_t getNormalCount();
  std::vector<float> getNormals();

  uint32_t getTextureCoordinateCount();
  std::vector<float> getTextureCoordinates();

  uint32_t getTotalIndexCount();
  tinyobj::index_t getTotalIndex(uint32_t index);

  uint32_t getTotalMaterialIndexCount();
  int getTotalMaterialIndex(uint32_t index);

  uint32_t getMaterialCount();
  tinyobj::material_t getMaterial(uint32_t index);

  uint32_t getPrimitiveCount();
};