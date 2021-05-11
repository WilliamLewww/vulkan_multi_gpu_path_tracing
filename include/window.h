#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

class Window {
private:
  GLFWwindow* window;
public:
  Window(int screenX, int screenY);
  ~Window();

  void setKeyCallback(void (*keyCallback)(GLFWwindow* window, int key, int scancode, int action, int mods));
  void setCursorPositionCallback(void (*cursorPositionCallback)(GLFWwindow* window, double xpos, double ypos));

  GLFWwindow* getWindow();
};