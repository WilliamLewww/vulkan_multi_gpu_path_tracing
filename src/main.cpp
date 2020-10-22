#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <stdio.h>
#include <unistd.h>

int main(void) {
  chdir("res");
  std::string inputfile = "bedroom.obj";
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warn;
  std::string err;

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

  if (!warn.empty()) {
    printf("%s\n", warn.c_str());
  }

  if (!err.empty()) {
    printf("%s\n", err.c_str());
  }

  if (!ret) {
    exit(EXIT_FAILURE);
  }
  chdir("..");

  exit(EXIT_SUCCESS);
}
