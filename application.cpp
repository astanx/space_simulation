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
  this->ViewMatrix = glm::lookAt(this->camPosition, this->camPosition + this->camFront, this->worldUp);
  if (this->framebufferHeight == 0)
  {
    std::cerr << "WARNING::FRAMEBUFFER_HEIGHT_ZERO, SETTING TO 1 TO AVOID DIVISION BY ZERO" << std::endl;
    this->framebufferHeight = 1;
  }
  this->ProjectionMatrix = glm::perspective(
      glm::radians(this->fov),
      static_cast<float>(this->framebufferWidth) / static_cast<float>(this->framebufferHeight),
      this->nearPlane,
      this->farPlane);
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

  this->shaders[CORE_SHADER]->setMat4fv(this->ViewMatrix, "ViewMatrix");
  this->shaders[CORE_SHADER]->setMat4fv(this->ProjectionMatrix, "ProjectionMatrix");

  for (size_t i = 0; i < this->lights.size(); i++)
  {
    this->lights[i]->sendToShader(*this->shaders[CORE_SHADER]);
  }

  this->shaders[CORE_SHADER]->setVec3f(this->camPosition, "camPosition");
  this->shaders[CORE_SHADER]->unuse();
}

void Application::updateUniforms()
{
  this->shaders[CORE_SHADER]->setMat4fv(this->ViewMatrix, "ViewMatrix");
  this->shaders[CORE_SHADER]->setMat4fv(this->ProjectionMatrix, "ProjectionMatrix");
  this->shaders[CORE_SHADER]->set1i(1, "isTexture");

  this->shaders[CORE_SHADER]->setVec3f(this->camPosition, "camPosition");

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
                                                                                                                                   GLminor(GLminor)
{
  // Init variables
  this->window = nullptr;
  this->framebufferWidth = 0;
  this->framebufferHeight = 0;

  this->fov = 45.f;
  this->nearPlane = 0.1f;
  this->farPlane = 100.f;

  this->worldUp = glm::vec3(0.f, 1.f, 0.f);
  this->camFront = glm::vec3(0.f, 0.f, -1.f);
  this->camPosition = glm::vec3(0.f, 0.f, 3.f);

  this->deltaTime = 0.f;
  this->lastFrame = 0.f;

  this->yaw = -90.f;
  this->pitch = 0.f;
  this->firstMouse = true;
  this->lastX = static_cast<float>(windowWidth) / 2.f;
  this->lastY = static_cast<float>(windowHeight) / 2.f;
  this->sensitivity = 0.1f;

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
}

void Application::render()
{
  // Clear
  glClearColor(0.f, 0.f, 0.f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->processInput();

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
  float cameraSpeed = 2.5f * this->deltaTime;
  if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(this->window, true);
  }
  if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
  {
    this->camPosition += cameraSpeed * this->camFront;
  }
  if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
  {
    this->camPosition -= cameraSpeed * this->camFront;
  }
  if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
  {
    this->camPosition -= glm::normalize(glm::cross(this->camFront, this->worldUp)) * cameraSpeed;
  }
  if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
  {
    this->camPosition += glm::normalize(glm::cross(this->camFront, this->worldUp)) * cameraSpeed;
  }
  if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    this->camPosition += cameraSpeed * this->worldUp;
  }
  if (glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
  {
    this->camPosition -= cameraSpeed * this->worldUp;
  }
  if (glfwGetKey(this->window, GLFW_KEY_L) == GLFW_PRESS)
  {
    this->lights[0]->move(this->camPosition);
  }
  this->ViewMatrix = glm::lookAt(this->camPosition, this->camPosition + this->camFront, this->worldUp);
}

// Static functions
void Application::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
  Application *appState = static_cast<Application *>(glfwGetWindowUserPointer(window));
  if (appState->firstMouse)
  {
    appState->lastX = xpos;
    appState->lastY = ypos;
    appState->firstMouse = false;
  }

  float xoffset = xpos - appState->lastX;
  float yoffset = appState->lastY - ypos;
  appState->lastX = xpos;
  appState->lastY = ypos;

  xoffset *= appState->sensitivity;
  yoffset *= appState->sensitivity;

  appState->yaw += xoffset;
  appState->pitch += yoffset;

  if (appState->pitch > 89.0f)
    appState->pitch = 89.0f;
  if (appState->pitch < -89.0f)
    appState->pitch = -89.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(appState->yaw)) * cos(glm::radians(appState->pitch));
  direction.y = sin(glm::radians(appState->pitch));
  direction.z = sin(glm::radians(appState->yaw)) * cos(glm::radians(appState->pitch));
  appState->camFront = glm::normalize(direction);

  appState->ViewMatrix = glm::lookAt(appState->camPosition, appState->camPosition + appState->camFront, appState->worldUp);
}

void Application::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
  Application *appState = static_cast<Application *>(glfwGetWindowUserPointer(window));

  appState->fov -= (float)yoffset;

  if (appState->fov < 1.0f)
    appState->fov = 1.0f;
  if (appState->fov > 45.0f)
    appState->fov = 45.0f;

  appState->ProjectionMatrix = glm::perspective(
      glm::radians(appState->fov),
      static_cast<float>(appState->framebufferWidth) / static_cast<float>(appState->framebufferHeight),
      appState->nearPlane,
      appState->farPlane);
}

void Application::framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{
  if (height == 0)
    return;

  Application *appState = static_cast<Application *>(glfwGetWindowUserPointer(window));

  appState->framebufferWidth = width;
  appState->framebufferHeight = height;

  appState->ProjectionMatrix = glm::perspective(
      glm::radians(appState->fov),
      static_cast<float>(width) / static_cast<float>(height),
      appState->nearPlane,
      appState->farPlane);

  glViewport(0, 0, width, height);
};
