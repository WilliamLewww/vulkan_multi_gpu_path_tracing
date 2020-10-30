#include "input.h"

std::vector<int> Input::keyDownList;

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    keyDownList.push_back(key);
  }
  if (action == GLFW_RELEASE) {
    keyDownList.erase(std::remove(keyDownList.begin(), keyDownList.end(), key), keyDownList.end());
  }
}

bool Input::checkKeyDown(int key) {
  return std::find(keyDownList.begin(), keyDownList.end(), key) != keyDownList.end();
}