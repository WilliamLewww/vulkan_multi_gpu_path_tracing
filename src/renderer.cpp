#include "renderer.h"

Renderer::Renderer(VkInstance vulkanInstance, VkSurfaceKHR surface) {
  this->deviceCollection = new DeviceCollection(vulkanInstance);

  Device* displayDevice = this->deviceCollection->getDevice(0);
  displayDevice->initializeDeviceQueue(surface);
}

Renderer::~Renderer() {
  delete this->deviceCollection;
}