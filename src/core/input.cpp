#include "core/input.h"

void Input::init(GLFWwindow *win)
{
  this->window = win;

  keys = {
      GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_B, GLFW_KEY_H,
      GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT,
      GLFW_KEY_ENTER, GLFW_KEY_ESCAPE, GLFW_KEY_UP,
      GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT};
}

void Input::update()
{
  previous = current;

  for (int &key : keys)
    this->current[key] = glfwGetKey(this->window, key) == GLFW_PRESS;
}

bool Input::isKeyHold(int key)
{
  return this->current[key];
}

bool Input::isKeyPressed(int key)
{
  return this->current[key] && !this->previous[key];
}

bool Input::isKeyReleased(int key)
{
  return !this->current[key] && this->previous[key];
}
