#pragma once

#include <stdio.h>
#include <unistd.h>

#include "tiny_obj_loader.h"

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
};