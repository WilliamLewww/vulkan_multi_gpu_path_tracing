#define TINYOBJLOADER_IMPLEMENTATION

#include "scene.h"
#include "camera.h"
#include "renderer.h"

int main(void) {
  Scene* scene = new Scene("cube_scene.obj");
  if (!scene->checkError()) {
    return EXIT_FAILURE;
  }

  Camera* camera = new Camera();
  Renderer* renderer = new Renderer(scene, camera);

  return EXIT_SUCCESS;
}
