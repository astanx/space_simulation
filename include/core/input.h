#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <vector>

class Input
{
private:
  GLFWwindow *window;
  std::unordered_map<int, bool> current;
  std::unordered_map<int, bool> previous;

  std::vector<int> keys;

public:
  Input() = default;
  ~Input() = default;

  void init(GLFWwindow *window);
  void update();

  bool isKeyHold(int key);
  bool isKeyPressed(int key);
  bool isKeyReleased(int key);
};
