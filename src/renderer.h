#pragma once

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "input.h"
#include "window.h"
#include "vulkan_instance.h"

class Renderer {
private:
  Window* window;
  VulkanInstance* vulkanInstance;
public:
  Renderer();
  ~Renderer();
};