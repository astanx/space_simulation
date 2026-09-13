#include "core/window/window.h"

#include "debug/logger.h"

#include "render/state/renderState.h"

#include <GL/glew.h>

// Private functions
void Window::initGLFW()
{
  if (!glfwInit())
  {
    glfwTerminate();
    Logger::logFatal("Window", "GLFW init failed");
  }
}
void Window::initWindow()
{
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->cfg.GLmajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->cfg.GLminor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
#endif

  glfwWindowHint(GLFW_RESIZABLE, this->cfg.resizable);

  GLFWwindow *window = glfwCreateWindow(this->cfg.width, this->cfg.height, this->cfg.title.c_str(), NULL, NULL);
  if (!window)
  {
    Logger::logFatal("Window", "GLFW window creation failed");
    glfwTerminate();
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  this->ctx.width = static_cast<float>(width);
  this->ctx.height = static_cast<float>(height);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, Window::framebufferResizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, Window::mouseCallback);
  glfwSetScrollCallback(window, Window::scrollCallback);

  glfwMakeContextCurrent(window);

  this->window = window;
}
void Window::initGLEW()
{
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    Logger::logFatal("Window", "GLEW init failed");
    glfwDestroyWindow(this->window);
    glfwTerminate();
  }
}
void Window::initOpenGLSettings()
{
  glEnable(GL_MULTISAMPLE);

  glEnable(GL_DEPTH_TEST);
  RenderState::applyDepthFunc();

  glEnable(GL_STENCIL_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Window::updateFrameContext()
{
  float aspect = 1.f;
  if (this->ctx.height != 0)
    aspect = static_cast<float>(this->ctx.width / this->ctx.height);

  this->ctx.aspect = aspect;
}

// Constructor / Destructor
Window::Window(const WindowConfig &cfg) : cfg(cfg) {};
Window::~Window()
{
  if (this->window)
  {
    glfwDestroyWindow(this->window);
    glfwTerminate();
  }
};

// Public functions
void Window::init()
{
  this->initGLFW();
  this->initWindow();
  this->initGLEW();
  this->initOpenGLSettings();

  this->updateFrameContext();
}
void Window::setWindowShouldClose()
{
  if (this->window)
    glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}
bool Window::getWindowShouldClose()
{
  if (this->window)
    return glfwWindowShouldClose(this->window);
  return false;
}
void Window::swapBuffers()
{
  if (this->window)
    glfwSwapBuffers(this->window);
}

void Window::setMouseCallback(std::function<void(float, float)> f)
{
  this->onMouseMove = f;
}
void Window::setScrollCallback(std::function<void(float)> f)
{
  this->onScroll = f;
}
void Window::setFramebufferResizeCallback(std::function<void(FrameContext)> f)
{
  this->onFramebufferResize = f;
}

// Static functions
void Window::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
  Window *w = static_cast<Window *>(glfwGetWindowUserPointer(window));

  w->onMouseMove(static_cast<float>(xpos), static_cast<float>(ypos));
}

void Window::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
  Window *w = static_cast<Window *>(glfwGetWindowUserPointer(window));

  w->onScroll(static_cast<float>(yoffset));
}

void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
  if (height == 0)
    return;

  Window *w = static_cast<Window *>(glfwGetWindowUserPointer(window));

  w->ctx.width = static_cast<float>(width);
  w->ctx.height = static_cast<float>(height);

  glViewport(0, 0, width, height);
  w->updateFrameContext();

  w->onFramebufferResize(w->ctx);
};
