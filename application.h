#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>

#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "mesh.h"
#include "light.h"
#include "primitives.h"


// Enums for easy access
enum shader { CORE_SHADER = 0 };
enum texture { CONTAINER_TEXTURE = 0, CONTAINER_SPECULAR_TEXTURE = 1 };
enum material { CONTAINER_MATERIAL = 0 };
enum mesh { CUBE_MESH = 0 };


class Application 
{
private:
  // Window properties
  GLFWwindow* window;
  const int windowWidth;
  const int windowHeight;
  int framebufferWidth;
  int framebufferHeight;

  // OpenGL properties
  const int GLmajor;
  const int GLminor;

  // Matrices
  float fov;
	float nearPlane;
	float farPlane;
  glm::mat4 ProjectionMatrix;

  glm::vec3 worldUp;
	glm::vec3 camFront;
	glm::vec3 camPosition;
  glm::mat4 ViewMatrix;

  // Timing
  float deltaTime;
  float lastFrame;

  //Camera control variables
  float yaw;
	float pitch;
	bool firstMouse;
	float lastX;
	float lastY;
	float sensitivity;

  // Shaders
  std::vector<Shader*> shaders;

  // Textures
  std::vector<Texture*> textures;

  // Materials
  std::vector<Material*> materials;

  // Meshes
  std::vector<Mesh*> meshes;

  // Lights
  std::vector<Light*> lights;

  // INITIALIZERS
  // GLFW and window
  void initGLFW();
  void initWindow(const char* title, GLboolean resizable);
  void initGLEW();
  void initOpenGLSettings();

  // Application components
  void initMatrices();
  void initShaders();
  void initTextures();
  void initMaterials();
  void initMeshes();
  void initLights();
  void initUniforms();


  void updateUniforms();
public:
  Application(
    const char* title, const int windowWidth, const int windowHeight, const int GLmajor, const int GLminor, GLboolean resizable
  );
  virtual ~Application();

  void render();
  void update();
  int getWindowShouldClose();
  void setWindowShouldClose();
  static void mouseCallback(GLFWwindow *window, double xpos, double ypos);
  static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
  static void framebuffer_resize_callback(GLFWwindow *window, int width, int height); 
  void processInput();
};