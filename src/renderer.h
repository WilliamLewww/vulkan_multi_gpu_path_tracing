#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "input.h"
#include "window.h"
#include "vulkan_instance.h"
#include "device_manager.h"
#include "camera.h"

class Renderer {
private:
  Window* window;
  VulkanInstance* vulkanInstance;
  DeviceManager* deviceManager;
public:
  Renderer(Scene* scene, Camera* camera);
  ~Renderer();
};