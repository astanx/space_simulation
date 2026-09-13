#pragma once

#include <GL/glew.h>

#include "core/app/appConfig.h"
#include "core/input/inputManager.h"
#include "core/window/window.h"
#include "core/time/timeManager.h"

#include "scene/scene.h"

#include "render/renderer/renderer.h"

#include "resources/manager/resourceManager.h"
#include "resources/threadPool/threadPool.h"

#include "debug/validators/validator.h"

#include <GLFW/glfw3.h>

#include <chrono>
#include <memory>

class Shader;
class Texture;
class Material;
class Mesh;
class Light;
class Model;

struct Radii;
struct HapkeParameters;
struct PhongMaterialProperties;

class Application
{
private:
  // Window
  std::unique_ptr<Window> window;

  // Config
  AppConfig cfg;
  bool isFinished = false;

  // Resource management
  ResourceManager resourceManager;
  ThreadPool threadPool;

  // Metrics
  bool isTextShown;

  // Renderer
  Renderer renderer;
  RenderContext renderCtx;

  // Input
  InputManager input;

  // Scene
  Scene scene;

  // Validator
  std::unique_ptr<Validator> validator;

  // Timing
  TimeManager timeManager;

  // INITIALIZERS
  void initWindow();
  void initResources();
  void initMode();

  void initWorld();
  void initRenderer();

  void processInput();

  void updateValidator();

public:
  Application(const AppConfig &config);
  virtual ~Application();

  void render();
  void update();
  int shouldExit();
};