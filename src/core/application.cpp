#include "core/application.h"
#include "graphics/primitives/cube.h"
#include "graphics/primitives/plane.h"
#include "graphics/primitives/circle.h"
#include "graphics/primitives/sphere.h"
#include "graphics/shader.h"
#include "physics/constants.h"
#include "scene/scene.h"

#include <iostream>

// Private functions
void Application::initGLFW()
{
  if (!glfwInit())
  {
    std::cerr << "ERROR::GLFW_INIT_FAILED" << std::endl;
    glfwTerminate();
    return;
  }
}
void Application::initWindow(const char *title, GLboolean resizable)
{
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->GLmajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->GLminor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
  glfwWindowHint(GLFW_RESIZABLE, resizable);

  GLFWwindow *window = glfwCreateWindow(this->windowWidth, this->windowHeight, title, NULL, NULL);
  if (!window)
  {
    std::cerr << "ERROR::GLFW_WINDOW_CREATION_FAILED" << std::endl;
    glfwTerminate();
    return;
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
    std::cerr << "ERROR::GLEW_INIT_FAILED" << std::endl;
    glfwDestroyWindow(this->window);
    glfwTerminate();
    return;
  }
}
void Application::initOpenGLSettings()
{
  glEnable(GL_DEPTH_TEST);

  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_STENCIL_TEST);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Constructor / Destructor
Application::Application(
    const char *title, const int windowWidth, const int windowHeight, const int GLmajor, const int GLminor, GLboolean resizable) : windowWidth(windowWidth),
                                                                                                                                   windowHeight(windowHeight),
                                                                                                                                   GLmajor(GLmajor),
                                                                                                                                   GLminor(GLminor),
                                                                                                                                   resourceManager(),
                                                                                                                                   scene(&resourceManager)

{
  // Init variables
  this->window = nullptr;
  this->framebufferWidth = 0;
  this->framebufferHeight = 0;

  this->deltaTime = 0.f;
  this->lastFrame = 0.f;

  // Initialize application
  this->initGLFW();
  this->initWindow(title, resizable);
  this->initGLEW();
  this->initOpenGLSettings();

  this->resourceManager.LoadShader(Res::CORE_SHADER, this->GLmajor, this->GLminor, "assets/shaders/vertex_core.glsl", "assets/shaders/fragment_core.glsl");

  // SUN
  Texture *sun_diff = this->resourceManager.LoadTexture(Res::SUN_DIFFUSE, "assets/textures/sun.png", GL_TEXTURE_2D);
  this->resourceManager.LoadMaterial(Res::SUN_MATERIAL, glm::vec3(0.1f),
                                     glm::vec3(0.9f, 0.5f, 0.4f),
                                     glm::vec3(0.3f),
                                     sun_diff, nullptr, 32.f);

  auto sun = std::make_unique<Sphere>(32, sunRadius * VISUAL_RADIUS_SCALE);
  this->resourceManager.LoadMesh(Res::SUN, std::move(sun));

  // Mercury
  Texture *mercury_diff = this->resourceManager.LoadTexture(Res::MERCURY_DIFFUSE, "assets/textures/mercury.png", GL_TEXTURE_2D);
  this->resourceManager.LoadMaterial(Res::MERCURY_MATERIAL, glm::vec3(0.1f),
                                     glm::vec3(0.9f, 0.5f, 0.4f),
                                     glm::vec3(0.3f),
                                     mercury_diff, nullptr, 32.f);

  auto mercury = std::make_unique<Sphere>(32, mercuryRadius * VISUAL_RADIUS_SCALE);
  this->resourceManager.LoadMesh(Res::MERCURY, std::move(mercury));

  // VENUS
  Texture *venus_diff = this->resourceManager.LoadTexture(Res::VENUS_DIFFUSE, "assets/textures/venus.png", GL_TEXTURE_2D);
  this->resourceManager.LoadMaterial(Res::VENUS_MATERIAL, glm::vec3(0.1f),
                                     glm::vec3(0.9f, 0.5f, 0.4f),
                                     glm::vec3(0.3f),
                                     venus_diff, nullptr, 32.f);

  auto venus = std::make_unique<Sphere>(32, venusRadius * VISUAL_RADIUS_SCALE);
  this->resourceManager.LoadMesh(Res::VENUS, std::move(venus));

  // EARTH
  Texture *earth_diff = this->resourceManager.LoadTexture(Res::EARTH_DIFFUSE, "assets/textures/earth.png", GL_TEXTURE_2D);
  this->resourceManager.LoadMaterial(Res::EARTH_MATERIAL, glm::vec3(0.1f),
                                     glm::vec3(0.9f, 0.5f, 0.4f),
                                     glm::vec3(0.3f),
                                     earth_diff, nullptr, 32.f);
  auto earth = std::make_unique<Sphere>(32, earthRadius * VISUAL_RADIUS_SCALE);
  this->resourceManager.LoadMesh(Res::EARTH, std::move(earth));

  

  // MOON
  Texture *moon_diff = this->resourceManager.LoadTexture(Res::MOON_DIFFUSE, "assets/textures/moon.png", GL_TEXTURE_2D);
  this->resourceManager.LoadMaterial(Res::MOON_MATERIAL, glm::vec3(0.1f),
                                     glm::vec3(0.9f, 0.5f, 0.4f),
                                     glm::vec3(0.3f),
                                     moon_diff, nullptr, 32.f);
  auto moon = std::make_unique<Sphere>(32, moonRadius * VISUAL_RADIUS_SCALE);
  this->resourceManager.LoadMesh(Res::MOON, std::move(moon));

  this->scene.init(windowWidth, windowHeight);
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

  // Poll events
  glfwPollEvents();

  this->processInput();
}

void Application::render()
{
  // Clear
  glClearColor(0.f, 0.f, 0.f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  Shader *core = this->resourceManager.GetShader(Res::CORE_SHADER);

  // Render scene
  this->scene.render(core, this->framebufferWidth, this->framebufferHeight, this->deltaTime);

  // Swap buffers
  glfwSwapBuffers(this->window);
}

void Application::processInput()
{
  if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    this->setWindowShouldClose();
  }
  if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
  {
    this->scene.processKeyboard(FORWARD, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
  {
    this->scene.processKeyboard(BACKWARD, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
  {
    this->scene.processKeyboard(LEFT, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
  {
    this->scene.processKeyboard(RIGHT, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    this->scene.processKeyboard(UP, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
  {
    this->scene.processKeyboard(DOWN, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_L) == GLFW_PRESS)
  {
    this->scene.getPointLights()[0]->move(this->scene.getActiveCameraPosition());
  }
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
