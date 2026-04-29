#include "camera/camera.h"

#include "render/frustum.h"

#include <glm/gtc/matrix_transform.hpp>

// Private functions
void Camera::updateCameraVectors()
{
  this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  this->front.y = sin(glm::radians(this->pitch));
  this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

  this->front = glm::normalize(this->front);
  this->right = glm::normalize(glm::cross(this->front, this->worldUp));
  this->up = glm::normalize(glm::cross(this->right, this->front));
}

float Camera::getAspect() const
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  float width = static_cast<float>(viewport[2]);
  float height = static_cast<float>(viewport[3]);

  float aspect = 1.f;
  if (height != 0)
    aspect = width / height;
  return aspect;
}

// Constructor and Destructor
Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 worldUp)
    : position(position), front(front), worldUp(worldUp), up(worldUp)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  float width = static_cast<float>(viewport[2]);
  float height = static_cast<float>(viewport[3]);

  this->mouseSensitivity = 0.2f;
  this->movementSpeed = 1.5f;

  this->right = glm::normalize(glm::cross(this->front, this->worldUp));

  this->yaw = -90.f;
  this->pitch = 0.f;
  this->roll = 0.f;

  this->firstMouse = true;
  this->lastX = static_cast<float>(width) / 2.f;
  this->lastY = static_cast<float>(height) / 2.f;

  this->fov = 45.f;
  this->nearPlane = 0.00001f;
  this->farPlane = 1000.f;

  this->updateCameraVectors();
}

// Getters
const glm::mat4 Camera::getViewMatrix() const
{
  return glm::lookAt(this->position, this->position + this->front, this->up);
}
const glm::mat4 Camera::getProjectionMatrix(float aspectRatio, float overrideFov) const
{
  aspectRatio = aspectRatio == -1.f ? this->getAspect() : aspectRatio;
  float fov = overrideFov == -1.0f ? this->fov : overrideFov;
  return glm::perspective(glm::radians(fov), aspectRatio, this->nearPlane, this->farPlane);
}
const glm::vec3 Camera::getPosition() const
{
  return this->position;
}

// Public functions
void Camera::processMouseScroll(float yoffset)
{
  this->fov -= yoffset;

  if (this->fov < 1.0f)
    this->fov = 1.0f;
  if (this->fov > 45.0f)
    this->fov = 45.0f;
}

void Camera::processMouseMovement(const float &xpos, const float &ypos)
{
  if (this->firstMouse)
  {
    this->lastX = xpos;
    this->lastY = ypos;
    this->firstMouse = false;
  }

  float xoffset = xpos - this->lastX;
  float yoffset = this->lastY - ypos;

  xoffset *= this->mouseSensitivity;
  yoffset *= this->mouseSensitivity;

  this->yaw += xoffset;
  this->pitch += yoffset;

  if (this->pitch > 89.0f)
    this->pitch = 89.0f;
  if (this->pitch < -89.0f)
    this->pitch = -89.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  direction.y = sin(glm::radians(this->pitch));
  direction.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  this->front = glm::normalize(direction);

  this->lastX = xpos;
  this->lastY = ypos;

  this->updateCameraVectors();
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
  switch (direction)
  {
  case FORWARD:
    this->position += this->front * this->movementSpeed * deltaTime;
    break;
  case BACKWARD:
    this->position -= this->front * this->movementSpeed * deltaTime;
    break;
  case LEFT:
    this->position -= this->right * this->movementSpeed * deltaTime;
    break;
  case RIGHT:
    this->position += this->right * this->movementSpeed * deltaTime;
    break;
  case UP:
    this->position += this->worldUp * this->movementSpeed * deltaTime;
    break;
  case DOWN:
    this->position -= this->worldUp * this->movementSpeed * deltaTime;
    break;
  default:
    break;
  }
}

const Frustum Camera::getFrustum(float aspectRatio) const
{
  aspectRatio = aspectRatio == -1.f ? this->getAspect() : aspectRatio;

  glm::mat4 m = glm::transpose(this->getProjectionMatrix(aspectRatio) * this->getViewMatrix());

  Frustum frustum;

  frustum.faces[LEFT_FACE] = m[3] + m[0];
  frustum.faces[RIGHT_FACE] = m[3] - m[0];
  frustum.faces[BOTTOM_FACE] = m[3] + m[1];
  frustum.faces[TOP_FACE] = m[3] - m[1];
  frustum.faces[NEAR_FACE] = m[3] + m[2];
  frustum.faces[FAR_FACE] = m[3] - m[2];

  for (auto &face : frustum.faces)
  {
    float length = glm::length(glm::vec3(face));
    face /= length;
  }

  return frustum;
}
