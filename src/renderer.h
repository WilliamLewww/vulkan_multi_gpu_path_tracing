#pragma once

#include <map>

#include "device_collection.h"
#include "model_collection.h"

class Renderer {
private:
  DeviceCollection* deviceCollection;
public:
  Renderer(VkInstance vulkanInstance, VkSurfaceKHR surface, ModelCollection* modelCollection);
  ~Renderer();
};