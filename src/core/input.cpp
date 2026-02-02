#include "core/input.h"

void Input::init(GLFWwindow *win)
{
  this->window = win;

  keys = {
      GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D,
      GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT,
      GLFW_KEY_ENTER, GLFW_KEY_ESCAPE};
}

void Input::update()
{
  previous = current;

  for (auto &key : keys)
    this->current[key] = glfwGetKey(this->window, key) == GLFW_PRESS;
}

bool Input::isKeyPressed(int key)
{
  return this->current[key];
}

bool Input::isKeyJustPressed(int key)
{
  return this->current[key] && !this->previous[key];
}

bool Input::isKeyJustReleased(int key)
{
  return !this->current[key] && this->previous[key];
}
