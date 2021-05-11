#pragma once

#include "model_collection.h"
#include "vulkan_instance.h"
#include "window.h"
#include "surface.h"
#include "input.h"
#include "renderer.h"
#include "camera.h"
#include "gui.h"

class Engine {
private:
  Camera* camera;
  Camera* minimapCamera;
  
  ModelCollection* modelCollection;
  Window* window;
  VulkanInstance* vulkanInstance;
  Surface* surface;
  Renderer* renderer;
  GUI* gui;

  bool mouseLocked = true;
public:
  Engine();
  ~Engine();

  void start();
};