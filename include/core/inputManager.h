#pragma once

#include <GLFW/glfw3.h>

#include <array>

enum class Action
{
  MoveForward = 0,
  MoveBackward,
  MoveLeft,
  MoveRight,
  MoveUp,
  MoveDown,

  IncreaseCameraSpeed,
  DecreaseCameraSpeed,

  ToggleBloom,
  ToggleHDR,
  Pause,

  LogPosition,
  Exit,

  DoubleTimestep,
  HalfTimestep,
  IncreaseTimestep,
  DecreaseTimestep,

  Count
};

class InputManager
{
private:
  std::array<bool, size_t(Action::Count)> current;
  std::array<bool, size_t(Action::Count)> previous;

  std::array<int, size_t(Action::Count)> keys;

  void assignKey(Action action, int key);

public:
  InputManager();
  ~InputManager() = default;

  void update(GLFWwindow *window);

  bool isActionHold(Action action);
  bool isActionPressed(Action action);
  bool isActionReleased(Action action);
};