#pragma once

#include <GLFW/glfw3.h>

class Input {
private:
public:
  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};