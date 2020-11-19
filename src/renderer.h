#pragma once
#include "device_collection.h"

class Renderer {
private:
  DeviceCollection* deviceCollection;
public:
  Renderer(VkInstance vulkanInstance);
  ~Renderer();
};