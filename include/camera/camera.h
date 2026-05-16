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
  double movementSpeed;
  double mouseSensitivity;

  glm::dvec3 worldUp;
  glm::dvec3 position;
  glm::dvec3 front;
  glm::dvec3 right;
  glm::dvec3 up;

  GLfloat yaw;
  GLfloat pitch;
  GLfloat roll;

  bool firstMouse;
  float lastX;
  float lastY;

  double fov;
  double nearPlane;
  double farPlane;

  void updateCameraVectors();

public:
  Camera(glm::dvec3 position, glm::dvec3 front, glm::dvec3 worldUp, float width, float height);
  ~Camera() {}

  // Getters
  const glm::dmat4 getViewMatrix() const;

  const glm::dmat4 getProjectionMatrix(double aspectRatio, double overrideFov = -1.0) const;

  const glm::dvec3 getPosition() const;

  void updateMovementSpeed(double incrementor);

  const double getNearPlane() const { return this->nearPlane; };
  const double getFarPlane() const { return this->farPlane; };

  // Functions
  void processMouseScroll(float yoffset);

  void processMouseMovement(const float &xpos, const float &ypos);

  void processKeyboard(CameraMovement direction, double deltaTime);

  const Frustum getFrustum(double aspectRatio) const;
};