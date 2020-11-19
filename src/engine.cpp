#include "engine.h"

Engine::Engine() {
  std::vector<std::string> modelFileNameList {"cube_scene.obj", "bedroom.obj"};
  this->modelCollection = new ModelCollection(modelFileNameList);

  this->window = new Window(800, 600);
  this->window->setKeyCallback(Input::keyCallback);
  this->window->setCursorPositionCallback(Input::cursorPositionCallback);

  std::vector<const char*> instanceExtensionList {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
  std::vector<const char*> instanceLayerList {"VK_LAYER_KHRONOS_validation"};
  this->vulkanInstance = new VulkanInstance("Vulkan Multi-GPU Ray Tracing", instanceExtensionList, instanceLayerList, true);

  this->surface = new Surface(this->vulkanInstance->getVulkanInstance(), this->window->getWindow());
}

Engine::~Engine() {
  delete this->surface;
  delete this->vulkanInstance;
  delete this->window;
  delete this->modelCollection;
}