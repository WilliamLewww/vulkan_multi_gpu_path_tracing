#pragma once

#include <map>

#include "device_collection.h"
#include "model_collection.h"
#include "camera.h"

class Renderer {
private:
  DeviceCollection* deviceCollection;
public:
  Renderer(VkInstance vulkanInstance, VkSurfaceKHR surface, ModelCollection* modelCollection, Camera* camera);
  ~Renderer();

  void render();
  void updateUniformBuffers(Camera* camera);
};