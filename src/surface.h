#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

class Surface {
private:
  VkSurfaceKHR surface;
public:
  Surface(VkInstance vulkanInstance, GLFWwindow* window);
  ~Surface();

  VkSurfaceKHR getSurface();
};