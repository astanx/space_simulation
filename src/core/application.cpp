#include "core/application.h"
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
  glfwWindowHint(GLFW_SAMPLES, 4);
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
  glEnable(GL_MULTISAMPLE);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
                                                                                                                                   scene(resourceManager, threadPool)

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

  // this->resourceManager.LoadShader(Res::CORE_SHADER, this->GLmajor, this->GLminor, "assets/shaders/vertex_core.glsl", "assets/shaders/debug/normal_fragment.glsl", "assets/shaders/debug/normal_geometry.glsl");
  this->resourceManager.LoadShader(Res::CORE_SHADER, this->GLmajor, this->GLminor, "assets/shaders/vertex_core.glsl", "assets/shaders/fragment_core.glsl");
  this->resourceManager.LoadShader(Res::SKYBOX_SHADER, this->GLmajor, this->GLminor, "assets/shaders/skybox/vertex.glsl", "assets/shaders/skybox/fragment.glsl");
  // this->resourceManager.LoadShader(Res::ASTEROID_SHADER, this->GLmajor, this->GLminor, "assets/shaders/asteroid/vertex.glsl", "assets/shaders/asteroid/fragment.glsl");
  this->resourceManager.LoadShader(Res::ASTEROID_SHADER, this->GLmajor, this->GLminor, "assets/shaders/asteroid/vertex_no_matrix.glsl", "assets/shaders/asteroid/fragment.glsl");
  this->resourceManager.LoadShader(Res::TRAIL_SHADER, this->GLmajor, this->GLminor, "assets/shaders/trail/vertex.glsl", "assets/shaders/trail/fragment.glsl");
  this->resourceManager.LoadShader(Res::POINT_SHADOW_SHADER, this->GLmajor, this->GLminor, "assets/shaders/shadow/point/vertex.glsl", "assets/shaders/shadow/point/fragment.glsl", "assets/shaders/shadow/point/geometry.glsl");
  // this->resourceManager.LoadShader(Res::DIRECTIONAL_SHADOW_SHADER, this->GLmajor, this->GLminor, "assets/shaders/shadow/directional/vertex.glsl", "assets/shaders/shadow/directional/fragment.glsl");

  loadCircularObject(Res::SUN, Res::SUN_DIFFUSE, "assets/textures/sun.png", Res::SUN_MATERIAL, sunRadius, glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0.f), 5 );
  loadCircularObject(Res::MERCURY, Res::MERCURY_DIFFUSE, "assets/textures/mercury.png", Res::MERCURY_MATERIAL, mercuryRadius, glm::vec3(0.05f), glm::vec3(0.7f, 0.6f, 0.55f), glm::vec3(0.15f), 16);
  loadCircularObject(Res::VENUS, Res::VENUS_DIFFUSE, "assets/textures/venus.png", Res::VENUS_MATERIAL, venusRadius, glm::vec3(0.12f), glm::vec3(0.95f, 0.85f, 0.65f), glm::vec3(0.4f, 0.35f, 0.25f), 12);
  loadCircularObject(Res::EARTH, Res::EARTH_DIFFUSE, "assets/textures/earth.png", Res::EARTH_MATERIAL, earthRadius, glm::vec3(0.08f), glm::vec3(0.9f, 0.95f, 1.f), glm::vec3(0.6f), 30, Res::EARTH_SPECULAR, "assets/textures/earth_specular.png");
  loadCircularObject(Res::MOON, Res::MOON_DIFFUSE, "assets/textures/moon.png", Res::MOON_MATERIAL, moonRadius, glm::vec3(0.04f), glm::vec3(0.55f), glm::vec3(0.08f), 13);
  loadCircularObject(Res::MARS, Res::MARS_DIFFUSE, "assets/textures/mars.png", Res::MARS_MATERIAL, marsRadius, glm::vec3(0.06f), glm::vec3(0.85f, 0.5f, 0.3f), glm::vec3(0.12f), 17);
  loadCircularObject(Res::JUPITER, Res::JUPITER_DIFFUSE, "assets/textures/jupiter.png", Res::JUPITER_MATERIAL, jupiterRadius, glm::vec3(0.1f), glm::vec3(0.95f, 0.9f, 0.85f), glm::vec3(0.25f), 25);

  Texture *diff = this->resourceManager.LoadTexture(Res::ASTEROID_DIFFUSE, "assets/textures/asteroid.png", GL_TEXTURE_2D);
  this->resourceManager.LoadAsteroidMaterial(Res::ASTEROID_MATERIAL, diff);

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

  // Update FPS counter
  this->frames++;
  float elapsed = currentFrame - lastFpsUpdateTime;
  if (elapsed >= 1.0f)
  {
    float fps = frames / elapsed;
    std::cout << "FPS: " << fps << std::endl;

    frames = 0;
    lastFpsUpdateTime = currentFrame;
  }

  float aspect = 1.0f;
  if (framebufferHeight > 0)
    aspect = static_cast<float>(framebufferWidth) / framebufferHeight;

  this->scene.update(this->deltaTime);
  this->scene.updateUBO(aspect);

  // Poll events
  glfwPollEvents();

  this->processInput();
}

void Application::render()
{
  // Clear
  glClearColor(1.f, 1.f, 1.f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  Shader *core = this->resourceManager.GetShader(Res::CORE_SHADER);
  Shader *skybox = this->resourceManager.GetShader(Res::SKYBOX_SHADER);
  Shader *asteroid = this->resourceManager.GetShader(Res::ASTEROID_SHADER);
  Shader *trail = this->resourceManager.GetShader(Res::TRAIL_SHADER);
  Shader *pointShadow = this->resourceManager.GetShader(Res::POINT_SHADOW_SHADER);

  // Render shadow map
  this->scene.renderPointShadow(pointShadow);

  glViewport(0, 0, this->framebufferWidth, this->framebufferHeight);

  // Render scene
  this->scene.render(core, this->framebufferWidth, this->framebufferHeight, this->deltaTime, skybox, asteroid, trail);

  // Render skybox
  this->scene.renderSkybox(skybox, this->framebufferWidth / this->framebufferHeight);

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

void Application::loadCircularObject(std::string name, std::string diffuse_name, const char *diffusePath, std::string material_name, double radius,
                                     glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess,
                                     std::string specular_name, const char *specularPath, int segments)
{
  Texture *diff = this->resourceManager.LoadTexture(diffuse_name, diffusePath, GL_TEXTURE_2D);
  this->resourceManager.LoadPhongMaterial(material_name, ambient, diffuse, specular,
                                          diff, nullptr, shininess);
  auto obj = std::make_unique<Sphere>(segments, radius * VISUAL_RADIUS_SCALE);
  this->resourceManager.LoadMesh(name, std::move(obj), VertexLayout::Full);
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
