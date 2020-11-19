#include "surface.h"

Surface::Surface(VkInstance vulkanInstance, GLFWwindow* window) {
  if (glfwCreateWindowSurface(vulkanInstance, window, NULL, &this->surface) != VK_SUCCESS) {
    printf("failed to create window surface\n");
  }
}

Surface::~Surface() {

}

VkSurfaceKHR Surface::getSurface() {
  return this->surface;
}