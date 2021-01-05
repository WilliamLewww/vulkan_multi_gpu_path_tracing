#include "engine.h"

Engine::Engine() {
  this->camera = new Camera();

  std::vector<std::string> modelFileNameList {"transparent.obj", "fun_lens.obj", "fish_eye_lens.obj"};
  this->modelCollection = new ModelCollection(modelFileNameList);

  this->window = new Window(800, 600);
  this->window->setKeyCallback(Input::keyCallback);
  this->window->setCursorPositionCallback(Input::cursorPositionCallback);

  std::vector<const char*> instanceExtensionList {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
  std::vector<const char*> instanceLayerList {"VK_LAYER_KHRONOS_validation"};
  this->vulkanInstance = new VulkanInstance("Vulkan Multi-GPU Ray Tracing", instanceExtensionList, instanceLayerList, true);

  this->surface = new Surface(this->vulkanInstance->getVulkanInstance(), this->window->getWindow());

  this->renderer = new Renderer(this->vulkanInstance->getVulkanInstance(), this->surface->getSurface(), this->modelCollection, this->camera);

  this->gui = new GUI(this->window->getWindow(),
                      this->vulkanInstance->getVulkanInstance(), 
                      this->renderer->getPhysicalDevice(), 
                      this->renderer->getLogicalDevice(), 
                      this->renderer->getGraphicsQueueIndex(), 
                      this->renderer->getGraphicsQueue(), 
                      this->renderer->getMinImageCount(),
                      this->renderer->getImageCount(),
                      this->renderer->getRenderPass(),
                      this->renderer->getCommandPool());
}

Engine::~Engine() {
  delete this->gui;
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

    if (Input::checkKeyPress(GLFW_KEY_Q)) {
      this->mouseLocked = !this->mouseLocked;

      if (this->mouseLocked) {
        this->camera->resetCursorPosition();
        glfwSetInputMode(this->window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }
      else {
        glfwSetInputMode(this->window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      }
    }

    if (this->mouseLocked) {
      this->camera->update();
    }
    else {
      this->camera->update(false);
    }

    this->renderer->updateCameraUniformBuffers(camera);
    this->gui->render(this->camera, this->renderer, this->renderer->getModelInstanceSet(0));
    this->renderer->render();
  }
}