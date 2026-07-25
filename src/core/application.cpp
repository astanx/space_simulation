#include "core/application.h"

#include "debug/logger.h"

#include "scene/scene.h"

#include "resources/resources.h"

#include "graphics/primitives/ellipsoid.h"
#include "graphics/primitives/quad.h"
#include "graphics/primitives/asteroidShape.h"

#include "render/renderState.h"

#include "physics/constants.h"

#include "maths/dateToJD.h"

#include <iostream>
#include <filesystem>

// Private functions
void Application::initGLFW()
{
  if (!glfwInit())
  {
    glfwTerminate();
    Logger::logFatal("Application", "GLFW init failed");
  }
}
void Application::initWindow(const char *title, GLboolean resizable)
{
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->GLmajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->GLminor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS

  glfwWindowHint(GLFW_RESIZABLE, resizable);

  GLFWwindow *window = glfwCreateWindow(this->windowWidth, this->windowHeight, title, NULL, NULL);
  if (!window)
  {
    Logger::logFatal("Application", "GLFW window creation failed");
    glfwTerminate();
  }

  glfwGetFramebufferSize(window, &this->framebufferWidth, &this->framebufferHeight);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, Application::framebuffer_resize_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, Application::mouseCallback);
  glfwSetScrollCallback(window, Application::scrollCallback);

  glfwMakeContextCurrent(window);

  this->window = window;
}
void Application::initGLEW()
{
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    Logger::logFatal("Application", "GLEW init failed");
    glfwDestroyWindow(this->window);
    glfwTerminate();
  }
}
void Application::initOpenGLSettings()
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

void Application::updateFrameContext()
{
  float aspect = 1.f;
  if (framebufferHeight != 0)
    aspect = static_cast<float>(framebufferWidth) / framebufferHeight;

  this->renderCtx.frameCtx.width = framebufferWidth;
  this->renderCtx.frameCtx.height = framebufferHeight;
  this->renderCtx.frameCtx.aspect = aspect;
}

// Constructor / Destructor
Application::Application(
    const char *title, const int windowWidth, const int windowHeight, const int GLmajor, const int GLminor, GLboolean resizable) : windowWidth(windowWidth),
                                                                                                                                   windowHeight(windowHeight),
                                                                                                                                   GLmajor(GLmajor),
                                                                                                                                   GLminor(GLminor),
                                                                                                                                   resourceManager(),
                                                                                                                                   threadPool(),
                                                                                                                                   scene(),
                                                                                                                                   input(),
                                                                                                                                   renderer(resourceManager)
{
  // Initialize application
  this->initGLFW();
  this->initWindow(title, resizable);
  this->initGLEW();
  this->initOpenGLSettings();

  // Init variables
  this->renderCtx.deltaTime = 0.f;
  this->renderCtx.settings.paused = false;
  this->renderCtx.settings.useBloom = true;
  this->renderCtx.settings.useHDR = true;
  this->renderCtx.settings.exposure = 5e-4;
  this->renderCtx.settings.bloomPower = 0.5;

  this->timeScale = 3600 * 24;
  this->deltaTime = 0.f;
  this->lastFrame = static_cast<float>(glfwGetTime());

  this->startTime = dateToJD(Date{1, 1, 1900}); // Day/Month/Year Hour:Minute:Second

  this->isTextShown = true;

  // this->resourceManager.LoadShader(Res::CORE_SHADER, this->GLmajor, this->GLminor, "assets/shaders/vertex_core.glsl", "assets/shaders/debug/normal_fragment.glsl", "assets/shaders/debug/normal_geometry.glsl");
  this->resourceManager.LoadShader(Res::CORE_SHADER, this->GLmajor, this->GLminor, "assets/shaders/vertex_core.glsl", "assets/shaders/fragment_core.glsl");
  this->resourceManager.LoadShader(Res::CORE_TANGENT_SHADER, this->GLmajor, this->GLminor, "assets/shaders/vertex_tangent_core.glsl", "assets/shaders/fragment_tangent_core.glsl");
  this->resourceManager.LoadShader(Res::SKYBOX_SHADER, this->GLmajor, this->GLminor, "assets/shaders/skybox/vertex.glsl", "assets/shaders/skybox/fragment.glsl");
  this->resourceManager.LoadShader(Res::TRAIL_SHADER, this->GLmajor, this->GLminor, "assets/shaders/trail/vertex.glsl", "assets/shaders/trail/fragment.glsl");
  this->resourceManager.LoadShader(Res::POINT_SHADOW_SHADER, this->GLmajor, this->GLminor, "assets/shaders/shadow/point/vertex.glsl", "assets/shaders/shadow/point/fragment.glsl", "assets/shaders/shadow/point/geometry.glsl");
  // this->resourceManager.LoadShader(Res::DIRECTIONAL_SHADOW_SHADER, this->GLmajor, this->GLminor, "assets/shaders/shadow/directional/vertex.glsl", "assets/shaders/shadow/directional/fragment.glsl");
  this->resourceManager.LoadShader(Res::TEXT_SHADER, this->GLmajor, this->GLminor, "assets/shaders/text/vertex.glsl", "assets/shaders/text/fragment.glsl");
  this->resourceManager.LoadShader(Res::HDR_SHADER, this->GLmajor, this->GLminor, "assets/shaders/hdr/vertex.glsl", "assets/shaders/hdr/fragment.glsl");
  this->resourceManager.LoadShader(Res::BLOOM_SHADER, this->GLmajor, this->GLminor, "assets/shaders/bloom/vertex.glsl", "assets/shaders/bloom/fragment.glsl");
  this->resourceManager.LoadShader(Res::BLUR_SHADER, this->GLmajor, this->GLminor, "assets/shaders/blur/texture/vertex.glsl", "assets/shaders/blur/texture/fragment.glsl");
  this->resourceManager.LoadShader(Res::BLUR_CUBEMAP_SHADER, this->GLmajor, this->GLminor, "assets/shaders/blur/cubemap/vertex.glsl", "assets/shaders/blur/cubemap/fragment.glsl", "assets/shaders/blur/cubemap/geometry.glsl");
  this->resourceManager.LoadShader(Res::CUBEMAP_SHADER, this->GLmajor, this->GLminor, "assets/shaders/cubemap/vertex.glsl", "assets/shaders/cubemap/fragment.glsl");
  this->resourceManager.LoadShader(Res::CONVOLUTION_SHADER, this->GLmajor, this->GLminor, "assets/shaders/convolution/vertex.glsl", "assets/shaders/convolution/fragment.glsl");
  this->resourceManager.LoadShader(Res::REFLECTION_SHADER, this->GLmajor, this->GLminor, "assets/shaders/reflector/vertex.glsl", "assets/shaders/reflector/fragment.glsl");
  this->resourceManager.LoadShader(Res::DOWNSAMPLE_SHADER, this->GLmajor, this->GLminor, "assets/shaders/sample/down/vertex.glsl", "assets/shaders/sample/down/fragment.glsl");
  this->resourceManager.LoadShader(Res::UPSAMPLE_SHADER, this->GLmajor, this->GLminor, "assets/shaders/sample/up/vertex.glsl", "assets/shaders/sample/up/fragment.glsl");
  this->resourceManager.LoadShader(Res::ATMOSPHERE_SHADER, this->GLmajor, this->GLminor, "assets/shaders/atmosphere/vertex.glsl", "assets/shaders/atmosphere/fragment.glsl");
  this->resourceManager.LoadShader(Res::IMPOSTOR_SHADER, this->GLmajor, this->GLminor, "assets/shaders/impostor/vertex.glsl", "assets/shaders/impostor/fragment.glsl");
  this->resourceManager.LoadShader(Res::POINT_SHADER, this->GLmajor, this->GLminor, "assets/shaders/point/vertex.glsl", "assets/shaders/point/fragment.glsl");

  // OpenCL
  this->resourceManager.LoadContext(Res::MAIN_CONTEXT);
  this->resourceManager.LoadProgram(Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM, "assets/kernels/wisdomHolman/wisdomHolman.cl", Res::MAIN_CONTEXT);
  this->resourceManager.LoadKernel(Res::DRIFT_ANGULAR_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);
  this->resourceManager.LoadKernel(Res::DRIFT_OBJECTS_LINEAR_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);
  this->resourceManager.LoadKernel(Res::DRIFT_ORBITAL_LINEAR_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);
  this->resourceManager.LoadKernel(Res::HALF_KICK_LINEAR_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);
  this->resourceManager.LoadKernel(Res::HALF_KICK_ANGULAR_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);
  this->resourceManager.LoadKernel(Res::HALF_KICK_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);

  this->loadEllipsoidObject(Res::SUN_MODEL, Res::SUN_MESH, Res::SUN_DIFFUSE, Res::SUN_MATERIAL, sunRadii, 1.f, 0.f, 0.05f, ModelFlags::None, sunLuminosity);
  // this->loadEllipsoidObject(Res::SUN, Res::SUN_DIFFUSE, Res::SUN_MATERIAL, sunRadii, 1.f, 0.f, 0.05f);
  this->loadEllipsoidObject(Res::MERCURY_MODEL, Res::MERCURY_MESH, Res::MERCURY_DIFFUSE, Res::MERCURY_MATERIAL, mercuryRadii, 0.9f, 0.f, 0.95f, ModelFlags::CastsShadow);
  this->loadEllipsoidObject(Res::VENUS_MODEL, Res::VENUS_MESH, Res::VENUS_DIFFUSE, Res::VENUS_MATERIAL, venusRadii, 0.9f, 0.f, 0.98f, ModelFlags::CastsShadow);
  this->loadEllipsoidObject(Res::VENUS_ATMOSPHERE_MODEL, Res::VENUS_ATMOSPHERE_MESH, Res::VENUS_ATMOSPHERE_DIFFUSE, Res::VENUS_ATMOSPHERE_MATERIAL, venusRadii.scaled(1.01), 1.f, 0.f, 0.05f);
  this->loadEllipsoidObject(Res::EARTH_MODEL, Res::EARTH_MESH, Res::EARTH_DIFFUSE, Res::EARTH_MATERIAL, earthRadii, 1.f, 0.f, 0.55f, ModelFlags::CastsShadow, 0.0f, Res::EARTH_NORMAL, Res::EARTH_NIGHT, Res::EARTH_ROUGHNESS);
  this->loadEllipsoidObject(Res::EARTH_ATMOSPHERE_MODEL, Res::EARTH_ATMOSPHERE_MESH, Res::EARTH_ATMOSPHERE_DIFFUSE, Res::EARTH_ATMOSPHERE_MATERIAL, earthRadii.scaled(1.01), 1.f, 0.f, 0.03f);
  this->loadEllipsoidObject(Res::MOON_MODEL, Res::MOON_MESH, Res::MOON_DIFFUSE, Res::MOON_MATERIAL, moonRadii, 0.95f, 0.f, 0.95f, ModelFlags::CastsShadow | ModelFlags::ReflectsLight);
  this->loadEllipsoidObject(Res::MARS_MODEL, Res::MARS_MESH, Res::MARS_DIFFUSE, Res::MARS_MATERIAL, marsRadii, 0.9f, 0.f, 0.9f, ModelFlags::CastsShadow);
  this->loadEllipsoidObject(Res::JUPITER_MODEL, Res::JUPITER_MESH, Res::JUPITER_DIFFUSE, Res::JUPITER_MATERIAL, jupiterRadii, 1.f, 0.f, 0.25f, ModelFlags::CastsShadow);

  Texture &diff = this->resourceManager.LoadTexture(Res::ASTEROID_DIFFUSE, BASE_TEXTURE_PATH + "diffuse/asteroid.png", GL_TEXTURE_2D);
  this->loadAsteroidShape(Res::EROS_ASTEROID, Res::EROS_ASTEROID_MODEL, Res::EROS_ASTEROID_MESH, Res::EROS_ASTEROID_MATERIAL, diff, 0.85f, 0.05f, 0.92f, 48, 32, 4.0, 1.0, 1.0, 2.5, 8.0, 8.0);
  this->loadAsteroidShape(Res::ITOKAWA_ASTEROID, Res::ITOKAWA_ASTEROID_MODEL, Res::ITOKAWA_ASTEROID_MESH, Res::ITOKAWA_ASTEROID_MATERIAL, diff, 0.9f, 0.08f, 0.95f, 64, 48, 7.0, 0.9, 1.1, 3.0, 12.0, 6.0);
  this->loadAsteroidShape(Res::BENNU_ASTEROID, Res::BENNU_ASTEROID_MODEL, Res::BENNU_ASTEROID_MESH, Res::BENNU_ASTEROID_MATERIAL, diff, 0.78f, 0.03f, 0.88f, 56, 40, 5.0, 1.0, 0.95, 1.8, 4.0, 10.0);
  this->loadAsteroidShape(Res::RYUGU_ASTEROID, Res::RYUGU_ASTEROID_MODEL, Res::RYUGU_ASTEROID_MESH, Res::RYUGU_ASTEROID_MATERIAL, diff, 0.82f, 0.06f, 0.85f, 52, 36, 3.0, 1.2, 0.8, 2.2, 5.0, 18.0);
  this->loadAsteroidShape(Res::VESTA_ASTEROID, Res::VESTA_ASTEROID_MODEL, Res::VESTA_ASTEROID_MESH, Res::VESTA_ASTEROID_MATERIAL, diff, 0.88f, 0.04f, 0.9f, 40, 28, 6.0, 1.0, 1.0, 4.0, 3.0, 15.0);

  this->resourceManager.LoadMesh<VertexPositionTexcoord>(Res::FULLSCREEN_QUAD, std::make_unique<Quad>(), VertexLayout::PositionTexcoord);

  this->updateFrameContext();

  this->scene.init(this->renderCtx, this->resourceManager, this->threadPool, this->startTime);
  this->renderer.init(this->scene, this->renderCtx);
}

Application::~Application()
{
  glfwDestroyWindow(this->window);
  glfwTerminate();
}

// Accessors
int Application::getWindowShouldClose()
{
  return glfwWindowShouldClose(this->window);
}

// Modifiers
void Application::setWindowShouldClose()
{
  glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}

// Public functions
void Application::update()
{
  // Calculate delta time
  float currentFrame = static_cast<float>(glfwGetTime());
  this->deltaTime = currentFrame - this->lastFrame;
  this->lastFrame = currentFrame;

  if (this->isFirstFrame)
  {
    this->deltaTime = 0.f;
    this->isFirstFrame = false;
  }

  // Update context
  this->renderCtx.deltaTime = this->deltaTime * this->timeScale;
  if (!this->renderCtx.settings.paused)
    this->elapsedDays += this->deltaTime * this->timeScale / 86400.0;

  // Update FPS counter
  this->frames++;
  float elapsed = currentFrame - lastFpsUpdateTime;
  if (elapsed >= 1.0f)
  {
    this->fps = frames / elapsed;

    Logger::logInfo("FPS", std::to_string(fps));

    this->frames = 0;
    this->lastFpsUpdateTime = currentFrame;
  }

  this->renderer.update(this->scene, this->renderCtx);

  // Poll events
  glfwPollEvents();

  this->input.update(this->window);

  this->processInput();
}

void Application::render()
{
  this->renderer.render(this->scene, this->renderCtx);

  if (this->isTextShown)
  {
    this->renderer.renderText("FPS: " + std::to_string(int(this->fps)),
                              25.f, this->framebufferHeight - 100.f, .5f, glm::vec3(0.5, 0.8f, 0.2f));
    this->renderer.renderText("Time scale: " + std::to_string(int(this->timeScale)) + " seconds per real second",
                              25.f, this->framebufferHeight - 150.f, .5f, glm::vec3(0.5, 0.8f, 0.2f));
    this->renderer.renderText("Date: " + JDToDate(this->startTime + this->elapsedDays).toString(),
                              25.f, this->framebufferHeight - 200.f, .5f, glm::vec3(0.5, 0.8f, 0.2f));

    if (this->renderCtx.settings.paused)
      this->renderer.renderText("Paused", this->framebufferWidth / 2 - 50.f, this->framebufferHeight - 100.f, .5f, glm::vec3(1.f, 0.8f, 0.2f));
  }

  // Swap buffers
  glfwSwapBuffers(this->window);
}

void Application::processInput()
{
  if (this->input.isActionPressed(Action::Exit))
    this->setWindowShouldClose();

  if (this->input.isActionHold(Action::MoveForward))
    this->scene.processKeyboard(FORWARD, this->deltaTime);

  if (this->input.isActionHold(Action::MoveBackward))
    this->scene.processKeyboard(BACKWARD, this->deltaTime);

  if (this->input.isActionHold(Action::MoveLeft))
    this->scene.processKeyboard(LEFT, this->deltaTime);

  if (this->input.isActionHold(Action::MoveRight))
    this->scene.processKeyboard(RIGHT, this->deltaTime);

  if (this->input.isActionHold(Action::MoveUp))
    this->scene.processKeyboard(UP, this->deltaTime);

  if (this->input.isActionHold(Action::MoveDown))
    this->scene.processKeyboard(DOWN, this->deltaTime);

  if (this->input.isActionPressed(Action::ToggleBloom))
    this->renderCtx.settings.useBloom = !this->renderCtx.settings.useBloom;

  if (this->input.isActionPressed(Action::ToggleHDR))
    this->renderCtx.settings.useHDR = !this->renderCtx.settings.useHDR;

  if (this->input.isActionPressed(Action::HideText))
    this->isTextShown = !this->isTextShown;

  if (this->input.isActionPressed(Action::LogPosition))
  {
    const glm::vec3 position = this->scene.getActiveCameraPosition();
    Logger::logInfo("Application", "Camera position: " +
                                       std::to_string(position.x) + ", " +
                                       std::to_string(position.y) + ", " +
                                       std::to_string(position.z));
  }

  if (this->input.isActionPressed(Action::Pause))
    this->renderCtx.settings.paused = !this->renderCtx.settings.paused;

  if (this->input.isActionPressed(Action::DecreaseCameraSpeed))
    this->scene.decreaseCameraSpeed();

  if (this->input.isActionPressed(Action::IncreaseCameraSpeed))
    this->scene.increaseCameraSpeed();

  if (this->input.isActionPressed(Action::DoubleTimestep))
  {
    if (this->timeScale > 0)
      this->timeScale *= 2;
    else
      this->timeScale *= .5;
  }

  if (this->input.isActionPressed(Action::HalfTimestep))
  {
    if (this->timeScale > 0)
      this->timeScale *= .5;
    else
      this->timeScale *= 2;
  }

  if (this->input.isActionHold(Action::DecreaseTimestep))
    this->timeScale -= 2;

  if (this->input.isActionHold(Action::IncreaseTimestep))
    this->timeScale += 2;
}
void Application::loadPBRMaterial(const std::string &model_name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                  float ao, float metallic, float roughness, float emissiveStrength, const std::string &normal_name, const std::string &night_name,
                                  const std::string &roughness_name)
{
  const std::string format = ".png";

  const std::string diffusePath = BASE_TEXTURE_PATH + "diffuse/" + model_name + format;

  if (!std::filesystem::exists(diffusePath))
  {
    Logger::logFatal("Application", "Diffuse texture is not found, skipping the object - " + model_name);
    return;
  }
  Texture &diff = this->resourceManager.LoadTexture(diffuse_name, diffusePath, GL_TEXTURE_2D);

  Texture *rough = nullptr;
  const std::string roughnessPath = BASE_TEXTURE_PATH + "roughness/" + model_name + format;
  if (std::filesystem::exists(roughnessPath) && roughness_name != "")
  {
    Logger::logInfo("Application", "Found roughness texture for object - " + model_name);
    rough = &this->resourceManager.LoadTexture(roughness_name, roughnessPath, GL_TEXTURE_2D);
  }

  Texture *normal = nullptr;
  const std::string normalPath = BASE_TEXTURE_PATH + "normal/" + model_name + format;
  if (std::filesystem::exists(normalPath) && normal_name != "")
  {
    Logger::logInfo("Application", "Found normal texture for object - " + model_name);
    normal = &this->resourceManager.LoadTexture(normal_name, normalPath, GL_TEXTURE_2D);
  }

  Texture *night = nullptr;
  const std::string nightPath = BASE_TEXTURE_PATH + "night/" + model_name + format;
  if (std::filesystem::exists(nightPath) && night_name != "")
  {
    Logger::logInfo("Application", "Found night texture for object - " + model_name);
    night = &this->resourceManager.LoadTexture(night_name, nightPath, GL_TEXTURE_2D);
  }

  this->resourceManager.LoadPBRMaterial(material_name, &diff, normal, nullptr, nullptr, rough, night, emissiveStrength, ao, metallic, roughness);
}
void Application::loadEllipsoidObject(const std::string &model_name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                      Radii radii, float ao, float metallic, float roughness, ModelFlags flags, float emissiveStrength, const std::string &normal_name, const std::string &night_name,
                                      const std::string &roughness_name, int segments)
{
  this->loadPBRMaterial(model_name, mesh_name, diffuse_name, material_name, ao, metallic, roughness, emissiveStrength, normal_name, night_name, roughness_name);

  bool isTangent = normal_name != "";

  std::unique_ptr<Ellipsoid> obj = std::make_unique<Ellipsoid>(segments, radii, isTangent);
  if (isTangent)
    this->resourceManager.LoadMesh<VertexPositionTexcoordNormalTangent>(mesh_name, std::move(obj),
                                                                        VertexLayout::PositionNormalTangent);
  else
    this->resourceManager.LoadMesh<VertexPositionTexcoordNormal>(mesh_name, std::move(obj),
                                                                 VertexLayout::NoColor);

  this->resourceManager.LoadModel(model_name, material_name, mesh_name, flags);
}

void Application::loadAsteroidShape(const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &material_name,
                                    Texture &albedo, float ao, float metallic, float roughness,
                                    double thetaSteps, double phiSteps, double m, double a, double b, double n1, double n2, double n3)
{
  std::unique_ptr<AsteroidShape> shape = std::make_unique<AsteroidShape>(thetaSteps, phiSteps, m, a, b, n1, n2, n3);

  Material &mat = this->resourceManager.LoadPBRMaterial(material_name, &albedo, nullptr, nullptr, nullptr, nullptr, nullptr, 0.f, ao, metallic, roughness);
  this->resourceManager.LoadAsteroid<VertexPositionTexcoordNormal>(name, model_name, mesh_name, std::move(shape), mat, VertexLayout::NoColor);
}

// Static functions
void Application::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
  Application *appState = static_cast<Application *>(glfwGetWindowUserPointer(window));

  appState->scene.processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
}

void Application::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
  Application *appState = static_cast<Application *>(glfwGetWindowUserPointer(window));

  appState->scene.processMouseScroll(static_cast<float>(yoffset));
}

void Application::framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{
  if (height == 0)
    return;

  Application *appState = static_cast<Application *>(glfwGetWindowUserPointer(window));

  appState->framebufferWidth = width;
  appState->framebufferHeight = height;

  glViewport(0, 0, width, height);

  appState->updateFrameContext();
  appState->renderer.resize(appState->renderCtx.frameCtx);
};
