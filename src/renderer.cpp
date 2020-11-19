#include "renderer.h"

Renderer::Renderer(VkInstance vulkanInstance) {
  this->deviceCollection = new DeviceCollection(vulkanInstance);
}

Renderer::~Renderer() {
  delete this->deviceCollection;
}