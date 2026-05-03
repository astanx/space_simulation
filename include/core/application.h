#pragma once

#include "scene/scene.h"
#include "render/renderer.h"
#include "core/input.h"
#include "resources/resourceManager.h"
#include "resources/threadPool.h"

#include <GLFW/glfw3.h>

class Shader;
class Texture;
class Material;
class Mesh;
class Light;
class Model;

struct Radii;
struct PhongMaterialProperties;

const std::string BASE_TEXTURE_PATH = "assets/textures/";

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
  bool useBloom;
  bool useHDR;
  double timeScale;
  float fps;

  // Renderer
  Renderer renderer;

  // Input
  Input input;

  // Scene
  Scene scene;

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
  void loadEllipsoidObject(const std::string &name, const std::string &diffuse_name, const std::string &material_name,
                           Radii radii, float emissiveStrength = 0.0f,
                           const std::string &normal_name = "", const std::string &specular_name = "", int segments = 32);
  void processInput();
};