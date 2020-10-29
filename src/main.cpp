#define TINYOBJLOADER_IMPLEMENTATION

#include "scene.h"
#include "renderer.h"

int main(void) {
  Scene* scene = new Scene("bedroom.obj");
  if (!scene->checkError()) {
    return EXIT_FAILURE;
  }

  Renderer* renderer = new Renderer(scene);

  return EXIT_SUCCESS;
}
