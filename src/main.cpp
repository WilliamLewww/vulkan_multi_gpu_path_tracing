#define TINYOBJLOADER_IMPLEMENTATION

#include <vector>

#include "model.h"
#include "camera.h"
#include "renderer.h"

int main(int argn, const char** argv) {
  std::vector<Model*> modelList;
  modelList.push_back(new Model("cube_scene.obj"));
  modelList.push_back(new Model("bedroom.obj"));

  Model* model = new Model("cube_scene.obj");
  if (!model->checkError()) {
    return EXIT_FAILURE;
  }

  Camera* camera = new Camera();
  Renderer* renderer = new Renderer(modelList, camera);

  return EXIT_SUCCESS;
}
