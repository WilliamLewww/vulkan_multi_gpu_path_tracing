#include "engine.h"

Engine::Engine() {
  this->camera = new Camera();

  std::vector<std::string> modelFileNameList {"cube_scene.obj", "transparent.obj"};
  this->modelCollection = new ModelCollection(modelFileNameList);

  this->window = new Window(800, 600);
  this->window->setKeyCallback(Input::keyCallback);
  this->window->setCursorPositionCallback(Input::cursorPositionCallback);

  std::vector<const char*> instanceExtensionList {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
  std::vector<const char*> instanceLayerList {"VK_LAYER_KHRONOS_validation"};
  this->vulkanInstance = new VulkanInstance("Vulkan Multi-GPU Ray Tracing", instanceExtensionList, instanceLayerList, true);

  this->surface = new Surface(this->vulkanInstance->getVulkanInstance(), this->window->getWindow());

  this->renderer = new Renderer(this->vulkanInstance->getVulkanInstance(), this->surface->getSurface(), this->modelCollection, this->camera);
}

Engine::~Engine() {
  delete this->renderer;
  delete this->surface;
  delete this->vulkanInstance;
  delete this->window;
  delete this->modelCollection;
  delete this->camera;
}

void Engine::start() {
  while (!glfwWindowShouldClose(this->window->getWindow())) {
    glfwPollEvents();
    this->camera->update();
    this->renderer->updateUniformBuffers(camera);
    this->renderer->render();
  }
}