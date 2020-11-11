#define TINYOBJLOADER_IMPLEMENTATION

#include "model.h"
#include "camera.h"
#include "renderer.h"

int main(int argn, const char** argv) {
  Model* model = new Model("cube_scene.obj");
  if (!model->checkError()) {
    return EXIT_FAILURE;
  }

  Camera* camera = new Camera();
  Renderer* renderer = new Renderer(model, camera);

  return EXIT_SUCCESS;
}
