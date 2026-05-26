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

// Constructor and Destructor
Camera::Camera(glm::dvec3 position, glm::dvec3 front, glm::dvec3 worldUp, float width, float height)
    : position(position), front(front), worldUp(worldUp), up(worldUp)
{
  this->mouseSensitivity = 0.2f;
  this->movementSpeed = 8000000000.0;

  this->yaw = -90.f;
  this->pitch = 0.f;
  this->roll = 0.f;

  this->firstMouse = true;
  this->lastX = width / 2.f;
  this->lastY = height / 2.f;

  this->fov = 45.f;
  this->nearPlane = 10000000.0;
  this->farPlane = 52.038 * 1.49597870700e11;

  this->updateCameraVectors();
}

// Getters
const glm::dmat4 Camera::getViewMatrix() const
{
  // return glm::lookAt(this->position, this->position + this->front, this->up);
  return glm::lookAt(glm::dvec3(0.0), this->front, this->up);
}
const glm::dmat4 Camera::getProjectionMatrix(double aspectRatio, double overrideFov) const
{
  double fov = overrideFov == -1.0 ? this->fov : overrideFov;
  return glm::perspective(glm::radians(fov), aspectRatio, this->nearPlane, this->farPlane);
}
const glm::dvec3 Camera::getPosition() const
{
  return this->position;
}

// Public functions
void Camera::updateMovementSpeed(double incrementor)
{
  if (incrementor < 0 && movementSpeed == 0)
    return;
  this->movementSpeed += incrementor;
};

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

void Camera::processKeyboard(CameraMovement direction, double deltaTime)
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

const Frustum Camera::getFrustum(double aspectRatio) const
{
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

glm::dvec3 Camera::worldToViewSpace(const glm::dvec3 &position) const
{
  return (glm::dvec3(
              position.x,
              -position.z, // Z - Y
              position.y   // Y - -Z
              ) -
          this->position);
}
