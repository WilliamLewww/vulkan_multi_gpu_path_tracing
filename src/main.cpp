#define TINYOBJLOADER_IMPLEMENTATION

#include "scene.h"
#include "camera.h"
#include "renderer.h"

int main(int argn, const char** argv) {
  Scene* scene = new Scene(argv[1]);
  if (!scene->checkError()) {
    return EXIT_FAILURE;
  }

  Camera* camera = new Camera();
  Renderer* renderer = new Renderer(scene, camera);

  return EXIT_SUCCESS;
}
