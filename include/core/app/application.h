#pragma once

#include <GL/glew.h>

#include "core/app/appConfig.h"
#include "core/input/inputManager.h"
#include "core/window/window.h"

#include "scene/scene.h"

#include "render/renderer/renderer.h"

#include "resources/resourceManager.h"
#include "resources/threadPool.h"

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

struct LoadedTextures
{
  Texture *diffuse = nullptr;
  Texture *roughness = nullptr;
  Texture *normal = nullptr;
  Texture *night = nullptr;
};

const std::string BASE_TEXTURE_PATH = "assets/textures/";

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
  float fps;
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
  std::chrono::steady_clock::time_point clock;
  double timestep;
  double deltaTime;
  double lastFrame;
  unsigned frames = 0;
  double lastFpsUpdateTime = 0.0;
  double elapsedDays = 0.0;
  double startTime = 0.0;
  bool isFirstFrame = true;

  double getTime();

  // INITIALIZERS
  void initWindow();
  void initResources();
  void initMode();

  void initShaderResources();
  void initKernelResources();
  void initModelResources();
  void initAsteroidResources();

  void initWorld();
  void initRenderer();

  LoadedTextures loadTextures(const std::string &name, const std::string &diffuse_name, const std::string &normal_name = "", const std::string &night_name = "", const std::string &roughness_name = "");
  void loadEllipsoid(const std::string &mesh_name, Radii radii, bool isTangent = false, int segments = 32);

  void loadHapkePBRMaterial(const std::string &name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                            float ao, float metallic, float roughness, HapkeParameters params, float emissiveStrength = 0.f, const std::string &normal_name = "", const std::string &night_name = "",
                            const std::string &roughness_name = "");

  void loadPBRMaterial(const std::string &name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                       float ao, float metallic, float roughness, float emissiveStrength = 0.f, const std::string &normal_name = "", const std::string &night_name = "",
                       const std::string &roughness_name = "");
  void loadEllipsoidObject(const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                           Radii radii, float ao, float metallic, float roughness, ModelFlags flags = ModelFlags::None, float emissiveStrength = 0.0f,
                           const std::string &normal_name = "", const std::string &night_name = "", const std::string &roughness_name = "", int segments = 32);
  void loadReflectanceAcceptorEllipsoidObject(const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                              Radii radii, float ao, float metallic, float roughness, ModelFlags flags = ModelFlags::None, float emissiveStrength = 0.0f,
                                              const std::string &normal_name = "", const std::string &night_name = "", const std::string &roughness_name = "", int segments = 32);
  void loadHapkeEllipsoidObject(const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                Radii radii, float ao, float metallic, float roughness, HapkeParameters hapke, const std::string &acceptor_model_name, ModelFlags flags = ModelFlags::None, float emissiveStrength = 0.0f,
                                const std::string &normal_name = "", const std::string &night_name = "", const std::string &roughness_name = "", int segments = 32);

  void loadAsteroidShape(const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &material_name,
                         Texture &albedo, float ao, float metallic, float roughness,
                         double thetaSteps, double phiSteps, double m, double a, double b, double n1, double n2, double n3);

  void processInput();

public:
  Application(const AppConfig &config);
  virtual ~Application();

  void render();
  void update();
  int shouldExit();
};