#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>

class Input {
private:
  static std::vector<int> keyDownList;
public:
  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

  static bool checkKeyDown(int key);
};