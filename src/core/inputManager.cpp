#include "core/inputManager.h"

// Private functions
void InputManager::assignKey(Action action, int key)
{
  this->keys[size_t(action)] = key;
}

// Constructor
InputManager::InputManager()
{
  this->assignKey(Action::MoveForward, GLFW_KEY_W);
  this->assignKey(Action::MoveBackward, GLFW_KEY_S);
  this->assignKey(Action::MoveRight, GLFW_KEY_D);
  this->assignKey(Action::MoveLeft, GLFW_KEY_A);
  this->assignKey(Action::MoveDown, GLFW_KEY_LEFT_SHIFT);
  this->assignKey(Action::MoveUp, GLFW_KEY_SPACE);

  this->assignKey(Action::Exit, GLFW_KEY_ESCAPE);
  this->assignKey(Action::Pause, GLFW_KEY_ENTER);
  this->assignKey(Action::LogPosition, GLFW_KEY_P);
  this->assignKey(Action::HideText, GLFW_KEY_T);

  this->assignKey(Action::ToggleHDR, GLFW_KEY_H);
  this->assignKey(Action::ToggleBloom, GLFW_KEY_B);

  this->assignKey(Action::IncreaseCameraSpeed, GLFW_KEY_EQUAL);
  this->assignKey(Action::DecreaseCameraSpeed, GLFW_KEY_MINUS);

  this->assignKey(Action::DoubleTimestep, GLFW_KEY_UP);
  this->assignKey(Action::HalfTimestep, GLFW_KEY_DOWN);
  this->assignKey(Action::IncreaseTimestep, GLFW_KEY_RIGHT);
  this->assignKey(Action::DecreaseTimestep, GLFW_KEY_LEFT);
}

// Public functions
void InputManager::update(GLFWwindow *window)
{
  this->previous = this->current;

  for (size_t i = 0; i < this->keys.size(); i++)
    this->current[i] = glfwGetKey(window, this->keys[i]) == GLFW_PRESS;
}

bool InputManager::isActionHold(Action action)
{
  return this->current[size_t(action)];
}
bool InputManager::isActionPressed(Action action)
{
  return this->current[size_t(action)] && !this->previous[size_t(action)];
}
bool InputManager::isActionReleased(Action action)
{
  return !this->current[size_t(action)] && this->previous[size_t(action)];
}