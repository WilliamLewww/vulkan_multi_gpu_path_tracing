#include "renderer.h"

Renderer::Renderer() {
  this->window = new Window(800, 600);
  this->window->setKeyCallback(Input::keyCallback);

  std::vector<const char*> instanceExtensionList {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
  std::vector<const char*> instanceLayerList {"VK_LAYER_KHRONOS_validation"};
  this->vulkanInstance = new VulkanInstance("Vulkan Multi-GPU Ray Tracing", instanceExtensionList, instanceLayerList, true);
  this->window->createWindowSurface(this->vulkanInstance->getInstance());
}

Renderer::~Renderer() {
  delete this->window;
}