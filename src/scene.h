#pragma once

#include <stdio.h>
#include <unistd.h>

#include "tiny_obj_loader.h"

struct Material {
  float ambient[3]; int padA;
  float diffuse[3]; int padB;
  float specular[3]; int padC;
  float emission[3]; int padD;
};

class Scene {
private:
  std::string fileName;
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warning;
  std::string error;
  bool success;
public:
  Scene(std::string fileName);
  ~Scene();

  bool checkError();

  uint32_t getVertexCount();
  std::vector<float> getVertices();

  uint32_t getTotalIndexCount();
  tinyobj::index_t getTotalIndex(uint32_t index);

  uint32_t getTotalMaterialIndexCount();
  int getTotalMaterialIndex(uint32_t index);

  uint32_t getMaterialCount();
  tinyobj::material_t getMaterial(uint32_t index);

  uint32_t getPrimitiveCount();
};