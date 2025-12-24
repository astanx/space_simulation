#include "application.h"

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

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
void Application::initMatrices()
{
  if (this->framebufferHeight == 0)
  {
    std::cerr << "WARNING::FRAMEBUFFER_HEIGHT_ZERO, SETTING TO 1 TO AVOID DIVISION BY ZERO" << std::endl;
    this->framebufferHeight = 1;
  }
  this->ProjectionMatrix = this->camera.getProjectionMatrix(static_cast<float>(this->framebufferWidth) / static_cast<float>(this->framebufferHeight));
}
void Application::initShaders()
{
  this->shaders.push_back(new Shader(this->GLmajor, this->GLminor, "shaders/vertex_core.glsl", "shaders/fragment_core.glsl"));
}
void Application::initTextures()
{
  this->textures.push_back(new Texture("textures/container.png", GL_TEXTURE_2D));
  this->textures.push_back(new Texture("textures/container_specular.png", GL_TEXTURE_2D));
}
void Application::initMaterials()
{
  if (this->textures.size() < 2)
  {
    std::cerr << "ERROR::NOT_ENOUGH_TEXTURES_TO_CREATE_MATERIAL" << std::endl;
    return;
  }
  this->materials.push_back(new Material(
      glm::vec3(0.1f),
      glm::vec3(1.f),
      glm::vec3(1.f),
      CONTAINER_TEXTURE,
      CONTAINER_SPECULAR_TEXTURE,
      32.f));
}
void Application::initMeshes()
{
  Cube cube;
  this->meshes.push_back(new Mesh(&cube));
}
void Application::initLights()
{
  this->lights.push_back(new Light(
      glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.8f), glm::vec3(2.f), glm::vec3(1.5f), 1.f, 0.09f, 0.032f));
}
void Application::initUniforms()
{
  this->shaders[CORE_SHADER]->use();

  this->shaders[CORE_SHADER]->setMat4fv(this->camera.getViewMatrix(), "ViewMatrix");
  this->shaders[CORE_SHADER]->setMat4fv(this->ProjectionMatrix, "ProjectionMatrix");

  for (size_t i = 0; i < this->lights.size(); i++)
  {
    this->lights[i]->sendToShader(*this->shaders[CORE_SHADER]);
  }

  this->shaders[CORE_SHADER]->unuse();
}

void Application::updateUniforms()
{
  this->shaders[CORE_SHADER]->setMat4fv(this->camera.getViewMatrix(), "ViewMatrix");
  this->shaders[CORE_SHADER]->setVec3f(this->camera.getPosition(), "camPosition");
  this->shaders[CORE_SHADER]->setMat4fv(this->ProjectionMatrix, "ProjectionMatrix");
  this->shaders[CORE_SHADER]->set1i(1, "isTexture");

  this->materials[CONTAINER_MATERIAL]->sendToShader(*this->shaders[CORE_SHADER]);

  for (size_t i = 0; i < this->lights.size(); i++)
  {
    this->lights[i]->sendToShader(*this->shaders[CORE_SHADER]);
  }
}

// Constructor / Destructor
Application::Application(
    const char *title, const int windowWidth, const int windowHeight, const int GLmajor, const int GLminor, GLboolean resizable) : windowWidth(windowWidth),
                                                                                                                                   windowHeight(windowHeight),
                                                                                                                                   GLmajor(GLmajor),
                                                                                                                                   GLminor(GLminor),
                                                                                                                                   camera(
                                                                                                                                       glm::vec3(0.f, 0.f, 3.f),
                                                                                                                                       glm::vec3(0.f, 0.f, -1.f),
                                                                                                                                       glm::vec3(0.f, 1.f, 0.f),
                                                                                                                                       static_cast<float>(windowWidth),
                                                                                                                                       static_cast<float>(windowHeight))
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
  this->initMatrices();
  this->initShaders();
  this->initTextures();
  this->initMaterials();
  this->initMeshes();
  this->initLights();
  this->initUniforms();
}

Application::~Application()
{
  glfwDestroyWindow(this->window);
  glfwTerminate();

  for (size_t i = 0; i < this->shaders.size(); i++)
  {
    delete this->shaders[i];
  }

  for (size_t i = 0; i < this->textures.size(); i++)
  {
    delete this->textures[i];
  }

  for (size_t i = 0; i < this->materials.size(); i++)
  {
    delete this->materials[i];
  }

  for (size_t i = 0; i < this->meshes.size(); i++)
  {
    delete this->meshes[i];
  }

  for (size_t i = 0; i < this->lights.size(); i++)
  {
    delete this->lights[i];
  }
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use shader
  this->shaders[CORE_SHADER]->use();

  // Update uniforms
  this->updateUniforms();

  // Render objects
  this->textures[CONTAINER_TEXTURE]->bind(CONTAINER_TEXTURE);
  this->textures[CONTAINER_SPECULAR_TEXTURE]->bind(CONTAINER_SPECULAR_TEXTURE);

  this->meshes[CUBE_MESH]->render(this->shaders[CORE_SHADER]);

  // Swap buffers
  glfwSwapBuffers(this->window);

  // Unbind everything
  glBindVertexArray(0);
  this->textures[CONTAINER_TEXTURE]->unbind();
  this->textures[CONTAINER_SPECULAR_TEXTURE]->unbind();
  this->shaders[CORE_SHADER]->unuse();
}

void Application::processInput()
{
  if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    this->setWindowShouldClose();
  }
  if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
  {
    this->camera.processKeyboard(FORWARD, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
  {
    this->camera.processKeyboard(BACKWARD, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
  {
    this->camera.processKeyboard(LEFT, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
  {
    this->camera.processKeyboard(RIGHT, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    this->camera.processKeyboard(UP, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
  {
    this->camera.processKeyboard(DOWN, this->deltaTime);
  }
  if (glfwGetKey(this->window, GLFW_KEY_L) == GLFW_PRESS)
  {
    this->lights[0]->move(this->camera.getPosition());
  }
}

// Static functions
void Application::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
  Application *appState = static_cast<Application *>(glfwGetWindowUserPointer(window));

  appState->camera.processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
}

void Application::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
  Application *appState = static_cast<Application *>(glfwGetWindowUserPointer(window));

  appState->camera.processMouseScroll(static_cast<float>(yoffset));

  appState->ProjectionMatrix = appState->camera.getProjectionMatrix(static_cast<float>(appState->framebufferWidth) / static_cast<float>(appState->framebufferHeight));
}

void Application::framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{
  if (height == 0)
    return;

  Application *appState = static_cast<Application *>(glfwGetWindowUserPointer(window));

  appState->framebufferWidth = width;
  appState->framebufferHeight = height;

  appState->ProjectionMatrix = appState->camera.getProjectionMatrix(static_cast<float>(width) / static_cast<float>(height));

  glViewport(0, 0, width, height);
};
