#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>

#include <stdio.h>

class Input {
private:
  static std::vector<int> keyDownList;
  static std::vector<int> keyPressList;
  static double cursorPositionX;
  static double cursorPositionY;
public:
  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

  static bool checkKeyDown(int key);
  static bool checkKeyPress(int key);

  static double getCursorPositionX();
  static double getCursorPositionY();
};