#pragma once

#include "model_collection.h"
#include "vulkan_instance.h"
#include "window.h"
#include "surface.h"
#include "input.h"
#include "renderer.h"
#include "camera.h"

class Engine {
private:
  Camera* camera;
  ModelCollection* modelCollection;
  Window* window;
  VulkanInstance* vulkanInstance;
  Surface* surface;
  Renderer* renderer;
public:
  Engine();
  ~Engine();
};