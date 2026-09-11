#pragma once

#include "core/window/windowConfig.h"

#include "scene/frameContext.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <functional>

class Window
{
private:
  GLFWwindow *window;
  const WindowConfig &cfg;

  FrameContext ctx;

  std::function<void(float, float)> onMouseMove;
  std::function<void(float)> onScroll;
  std::function<void(FrameContext)> onFramebufferResize;

  void initGLFW();
  void initWindow();
  void initGLEW();
  void initOpenGLSettings();

  void updateFrameContext();

public:
  Window(const WindowConfig &cfg);
  ~Window();

  void init();

  void setWindowShouldClose();
  bool getWindowShouldClose();
  void swapBuffers();

  void setMouseCallback(std::function<void(float, float)> f);
  void setScrollCallback(std::function<void(float)> f);
  void setFramebufferResizeCallback(std::function<void(FrameContext)> f);

  static void mouseCallback(GLFWwindow *window, double xpos, double ypos);
  static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
  static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

  GLFWwindow *get() { return this->window; };
  FrameContext getFrameContext() { return this->ctx; };
  int getWidth() { return this->ctx.width; };
  int getHeight() { return this->ctx.height; };
};