#pragma once

#include "model_collection.h"
#include "vulkan_instance.h"
#include "window.h"
#include "surface.h"
#include "input.h"

class Engine {
private:
  ModelCollection* modelCollection;
  Window* window;
  VulkanInstance* vulkanInstance;
  Surface* surface;
public:
  Engine();
  ~Engine();
};