#pragma once

#include "scene/scene.h"
#include "scene/textRenderer.h"
#include "core/input.h"
#include "resources/resourceManager.h"
#include "resources/threadPool.h"
#include "physics/structs/radii.h"

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader;
class Texture;
class Material;
class Mesh;
class Light;
class Model;

class Application
{
private:
  // Window properties
  GLFWwindow *window;
  const int windowWidth;
  const int windowHeight;
  int framebufferWidth;
  int framebufferHeight;

  // OpenGL properties
  const int GLmajor;
  const int GLminor;

  // Resource management
  ResourceManager resourceManager;
  ThreadPool threadPool;

  // Metrics
  bool paused;
  double timeScale;
  float fps;

  // Input
  Input input;

  // Scene
  Scene scene;
  TextRenderer textRenderer;

  // Timing
  float deltaTime;
  float lastFrame;
  unsigned frames = 0;
  float lastFpsUpdateTime = 0.0f;

  // INITIALIZERS
  // GLFW and window
  void initGLFW();
  void initWindow(const char *title, GLboolean resizable);
  void initGLEW();
  void initOpenGLSettings();

public:
  Application(
      const char *title, const int windowWidth, const int windowHeight, const int GLmajor, const int GLminor, GLboolean resizable);
  virtual ~Application();

  void render();
  void update();
  int getWindowShouldClose();
  void setWindowShouldClose();
  static void mouseCallback(GLFWwindow *window, double xpos, double ypos);
  static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
  static void framebuffer_resize_callback(GLFWwindow *window, int width, int height);
  void loadEllipsoidObject(std::string name, std::string diffuse_name, const char *diffusePath, std::string material_name,
                           Radii radii, MaterialProperties material,
                           std::string specular_name = "", const char *specularPath = nullptr, int segments = 32);
  void processInput();
};