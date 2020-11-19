#include "input.h"

std::vector<int> Input::keyDownList;

double Input::cursorPositionX;
double Input::cursorPositionY;

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    keyDownList.push_back(key);
  }
  if (action == GLFW_RELEASE) {
    keyDownList.erase(std::remove(keyDownList.begin(), keyDownList.end(), key), keyDownList.end());
  }
}

void Input::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
  cursorPositionX = xpos;
  cursorPositionY = ypos;
}

bool Input::checkKeyDown(int key) {
  return std::find(keyDownList.begin(), keyDownList.end(), key) != keyDownList.end();
}

double Input::getCursorPositionX() {
  return cursorPositionX;
}

double Input::getCursorPositionY() {
  return cursorPositionY;
}