#include "window.h"

Window::Window(int screenX, int screenY) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  this->window = glfwCreateWindow(screenX, screenY, "Vulkan", NULL, NULL);

  glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Window::~Window() {

}

void Window::setKeyCallback(void (*keyCallback)(GLFWwindow* window, int key, int scancode, int action, int mods)) {
  glfwSetKeyCallback(this->window, keyCallback);
}

void Window::setCursorPositionCallback(void (*cursorPositionCallback)(GLFWwindow* window, double xpos, double ypos)) {
  glfwSetCursorPosCallback(this->window, cursorPositionCallback);
}

void Window::createWindowSurface(VkInstance vulkanInstance) {
  if (glfwCreateWindowSurface(vulkanInstance, this->window, NULL, &this->surface) == VK_SUCCESS) {
    printf("created window surface\n");
  }
}

GLFWwindow* Window::getWindow() {
  return this->window;
}

VkSurfaceKHR Window::getSurface() {
  return this->surface;
}