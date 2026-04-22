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

struct Frustum;

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
  float getAspect() const;

public:
  Camera(glm::vec3 position, glm::vec3 front, glm::vec3 worldUp);
  ~Camera() {}

  // Getters
  const glm::mat4 getViewMatrix() const;

  const glm::mat4 getProjectionMatrix(float aspectRatio = -1.f, float overrideFov = -1.f) const;

  const glm::vec3 getPosition() const;

  const float getNearPlane() const { return this->nearPlane; };
  const float getFarPlane() const { return this->farPlane; };

  // Functions
  void processMouseScroll(float yoffset);

  void processMouseMovement(const float &xpos, const float &ypos);

  void processKeyboard(CameraMovement direction, float deltaTime);

  const Frustum getFrustum(float aspectRatio = -1.f) const;
};