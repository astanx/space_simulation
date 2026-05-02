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

public:
  Camera(glm::vec3 position, glm::vec3 front, glm::vec3 worldUp, float width, float height);
  ~Camera() {}

  // Getters
  const glm::mat4 getViewMatrix() const;

  const glm::mat4 getProjectionMatrix(float aspectRatio, float overrideFov = -1.f) const;

  const glm::vec3 getPosition() const;

  void updateMovementSpeed(float incrementor) { this->movementSpeed += incrementor; };

  const float getNearPlane() const { return this->nearPlane; };
  const float getFarPlane() const { return this->farPlane; };

  // Functions
  void processMouseScroll(float yoffset);

  void processMouseMovement(const float &xpos, const float &ypos);

  void processKeyboard(CameraMovement direction, float deltaTime);

  const Frustum getFrustum(float aspectRatio) const;
};