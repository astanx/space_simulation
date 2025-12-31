#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

enum CameraMovement
{
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN
};

class Camera
{
private:
  GLfloat movementSpeed;
  GLfloat mouseSensitivity;

  glm::vec3 worldUp;
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 right;
  glm::vec3 up;

  GLfloat yaw;
  GLfloat pitch;
  GLfloat roll;

  bool firstMouse;
  float lastX;
  float lastY;

  float fov;
  float nearPlane;
  float farPlane;

  void updateCameraVectors();

public:
  Camera(glm::vec3 position, glm::vec3 front, glm::vec3 worldUp, float windowWidth, float windowHeight);
  ~Camera() {}

  // Getters
  const glm::mat4 getViewMatrix();

  const glm::mat4 getProjectionMatrix(float aspectRatio);

  const glm::vec3 getPosition() const;

  // Functions
  void processMouseScroll(float yoffset);

  void processMouseMovement(const float &xpos, const float &ypos);

  void processKeyboard(CameraMovement direction, float deltaTime);
};