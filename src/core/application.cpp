#include "core/application.h"

#include "debug/logger.h"

#include "scene/scene.h"

#include "resources/resources.h"

#include "graphics/primitives/ellipsoid.h"

#include "physics/constants.h"

#include <iostream>
#include <filesystem>

// Private functions
void Application::initGLFW()
{
  if (!glfwInit())
  {
    glfwTerminate();
    throw std::runtime_error("[Application] RUNTIME ERROR: GLFW init failed");
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
    glfwTerminate();
    throw std::runtime_error("[Application] RUNTIME ERROR: GLFW window creation failed");
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
    glfwDestroyWindow(this->window);
    glfwTerminate();
    throw std::runtime_error("[Application] RUNTIME ERROR: GLEW init failed");
  }
}
void Application::initOpenGLSettings()
{
  glEnable(GL_MULTISAMPLE);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Constructor / Destructor
Application::Application(
    const char *title, const int windowWidth, const int windowHeight, const int GLmajor, const int GLminor, GLboolean resizable) : windowWidth(windowWidth),
                                                                                                                                   windowHeight(windowHeight),
                                                                                                                                   GLmajor(GLmajor),
                                                                                                                                   GLminor(GLminor),
                                                                                                                                   resourceManager(),
                                                                                                                                   threadPool(),
                                                                                                                                   scene(resourceManager, threadPool),
                                                                                                                                   input(),
                                                                                                                                   renderer(resourceManager)

{
  // Init variables
  this->window = nullptr;
  this->framebufferWidth = 0;
  this->framebufferHeight = 0;

  this->deltaTime = 0.f;
  this->lastFrame = 0.f;
  this->timeScale = 3600 * 24;

  this->paused = false;
  this->useBloom = true;
  this->useHDR = true;

  // Initialize application
  this->initGLFW();
  this->initWindow(title, resizable);
  this->initGLEW();
  this->initOpenGLSettings();

  this->input.init(this->window);

  // this->resourceManager.LoadShader(Res::CORE_SHADER, this->GLmajor, this->GLminor, "assets/shaders/vertex_core.glsl", "assets/shaders/debug/normal_fragment.glsl", "assets/shaders/debug/normal_geometry.glsl");
  this->resourceManager.LoadShader(Res::CORE_SHADER, this->GLmajor, this->GLminor, "assets/shaders/vertex_core.glsl", "assets/shaders/fragment_core.glsl");
  this->resourceManager.LoadShader(Res::SKYBOX_SHADER, this->GLmajor, this->GLminor, "assets/shaders/skybox/vertex.glsl", "assets/shaders/skybox/fragment.glsl");
  // this->resourceManager.LoadShader(Res::ASTEROID_SHADER, this->GLmajor, this->GLminor, "assets/shaders/asteroid/vertex.glsl", "assets/shaders/asteroid/fragment.glsl");
  this->resourceManager.LoadShader(Res::ASTEROID_SHADER, this->GLmajor, this->GLminor, "assets/shaders/asteroid/vertex_no_matrix.glsl", "assets/shaders/asteroid/fragment.glsl");
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

  loadEllipsoidObject(Res::SUN, Res::SUN_DIFFUSE, Res::SUN_MATERIAL, sunRadii, sunMaterial, 1.2e5f);
  loadEllipsoidObject(Res::MERCURY, Res::MERCURY_DIFFUSE, Res::MERCURY_MATERIAL, mercuryRadii, mercuryMaterial);
  loadEllipsoidObject(Res::VENUS, Res::VENUS_DIFFUSE, Res::VENUS_MATERIAL, venusRadii, venusMaterial);
  loadEllipsoidObject(Res::EARTH, Res::EARTH_DIFFUSE, Res::EARTH_MATERIAL, earthRadii, earthMaterial, 0.0f, Res::EARTH_NORMAL);
  loadEllipsoidObject(Res::MOON, Res::MOON_DIFFUSE, Res::MOON_MATERIAL, moonRadii, moonMaterial);
  loadEllipsoidObject(Res::MARS, Res::MARS_DIFFUSE, Res::MARS_MATERIAL, marsRadii, marsMaterial);
  loadEllipsoidObject(Res::JUPITER, Res::JUPITER_DIFFUSE, Res::JUPITER_MATERIAL, jupiterRadii, jupiterMaterial);

  Texture &diff = this->resourceManager.LoadTexture(Res::ASTEROID_DIFFUSE, "assets/textures/diffuse/asteroid.png", GL_TEXTURE_2D);
  this->resourceManager.LoadAsteroidMaterial(Res::ASTEROID_MATERIAL, diff);

  this->scene.init();
  this->renderer.init(this->scene);
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

  this->renderer.update(this->scene, this->deltaTime * timeScale, this->paused);

  // Poll events
  glfwPollEvents();

  this->input.update();

  this->processInput();
}

void Application::render()
{
  // Clear
  glClearColor(1.f, 1.f, 1.f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  this->renderer.render(this->scene, this->useBloom, this->useHDR);

  this->renderer.renderText("FPS: " + std::to_string(int(this->fps)),
                            25.f, this->framebufferHeight - 100.f, .5f, glm::vec3(0.5, 0.8f, 0.2f));
  this->renderer.renderText("Time scale: " + std::to_string(int(this->timeScale)) + " seconds per frame",
                            25.f, this->framebufferHeight - 150.f, .5f, glm::vec3(0.5, 0.8f, 0.2f));

  // Swap buffers
  glfwSwapBuffers(this->window);
}

void Application::processInput()
{
  if (this->input.isKeyPressed(GLFW_KEY_ESCAPE))
  {
    this->setWindowShouldClose();
  }
  if (this->input.isKeyHold(GLFW_KEY_W))
  {
    this->scene.processKeyboard(FORWARD, this->deltaTime);
  }
  if (this->input.isKeyHold(GLFW_KEY_S))
  {
    this->scene.processKeyboard(BACKWARD, this->deltaTime);
  }
  if (this->input.isKeyHold(GLFW_KEY_A))
  {
    this->scene.processKeyboard(LEFT, this->deltaTime);
  }
  if (this->input.isKeyHold(GLFW_KEY_D))
  {
    this->scene.processKeyboard(RIGHT, this->deltaTime);
  }
  if (this->input.isKeyHold(GLFW_KEY_SPACE))
  {
    this->scene.processKeyboard(UP, this->deltaTime);
  }
  if (this->input.isKeyHold(GLFW_KEY_LEFT_SHIFT))
  {
    this->scene.processKeyboard(DOWN, this->deltaTime);
  }

  if (this->input.isKeyPressed(GLFW_KEY_B))
  {
    this->useBloom = !this->useBloom;
  }
  if (this->input.isKeyPressed(GLFW_KEY_H))
  {
    this->useHDR = !this->useHDR;
  }
  if (this->input.isKeyPressed(GLFW_KEY_P))
  {
    const glm::vec3 position = this->scene.getActiveCameraPosition();
    Logger::logInfo("Application", "Camera position: " +
        std::to_string(position.x) + ", " +
        std::to_string(position.y) + ", " +
        std::to_string(position.z));
  }

  if (this->input.isKeyPressed(GLFW_KEY_ENTER))
  {
    this->paused = !this->paused;
  }

  if (this->input.isKeyPressed(GLFW_KEY_UP))
  {
    if (this->timeScale > 0)
      this->timeScale *= 2;
    else
      this->timeScale *= .5;
  }
  if (this->input.isKeyPressed(GLFW_KEY_DOWN))
  {
    if (this->timeScale > 0)
      this->timeScale *= .5;
    else
      this->timeScale *= 2;
  }
  if (this->input.isKeyHold(GLFW_KEY_LEFT))
  {
    this->timeScale -= 2;
  }
  if (this->input.isKeyHold(GLFW_KEY_RIGHT))
  {
    this->timeScale += 2;
  }
}

void Application::loadEllipsoidObject(const std::string &name, const std::string &diffuse_name, const std::string &material_name,
                                      Radii radii, PhongMaterialProperties material, float emissiveStrength, const std::string &normal_name,
                                      const std::string &specular_name, int segments)
{
  const std::string format = ".png";

  const std::string diffusePath = BASE_TEXTURE_PATH + "diffuse/" + name + format;

  if (!std::filesystem::exists(diffusePath))
  {
    Logger::logFatal("Application", "Diffuse texture is not found, skipping the object - " + name);
    return;
  }
  Texture &diff = this->resourceManager.LoadTexture(diffuse_name, diffusePath, GL_TEXTURE_2D);

  Texture *spec = nullptr;
  const std::string specularPath = BASE_TEXTURE_PATH + "specular/" + name + format;
  if (std::filesystem::exists(specularPath) && specular_name != "")
  {
    Logger::logInfo("Application", "Found specular texture for object - " + name);
    spec = &this->resourceManager.LoadTexture(specular_name, specularPath, GL_TEXTURE_2D);
  }

  Texture *normal = nullptr;
  const std::string normalPath = BASE_TEXTURE_PATH + "normal/" + name + format;
  if (std::filesystem::exists(normalPath) && normal_name != "")
  {
    Logger::logInfo("Application", "Found normal texture for object - " + name);
    normal = &this->resourceManager.LoadTexture(normal_name, normalPath, GL_TEXTURE_2D);
  }

  bool isTangent = normal_name != "";

  // this->resourceManager.LoadPhongMaterial(material_name, material, &diff, spec, normal);
  this->resourceManager.LoadPBRMaterial(material_name, &diff, normal, nullptr, nullptr, nullptr, emissiveStrength);
  std::unique_ptr<Ellipsoid> obj = std::make_unique<Ellipsoid>(segments, radii.scaled(VISUAL_RADIUS_SCALE), isTangent);
  this->resourceManager.LoadMesh(name, std::move(obj),
                                 isTangent ? VertexLayout::PositionNormalTangent : VertexLayout::NoColor);
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
};
