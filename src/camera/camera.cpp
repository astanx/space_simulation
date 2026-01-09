#include "camera/camera.h"

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
Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 worldUp, float windowWidth, float windowHeight)
    : position(position), front(front), worldUp(worldUp), up(worldUp)
{
  this->mouseSensitivity = 0.2f;
  this->movementSpeed = 250.5f;

  this->right = glm::normalize(glm::cross(this->front, this->worldUp));

  this->yaw = -90.f;
  this->pitch = 0.f;
  this->roll = 0.f;

  this->firstMouse = true;
  this->lastX = static_cast<float>(windowWidth) / 2.f;
  this->lastY = static_cast<float>(windowHeight) / 2.f;

  this->fov = 45.f;
  this->nearPlane = 0.1f;
  this->farPlane = 10000000000.f;

  this->updateCameraVectors();
}

// Getters
const glm::mat4 Camera::getViewMatrix()
{
  return glm::lookAt(this->position, this->position + this->front, this->up);
}
const glm::mat4 Camera::getProjectionMatrix(float aspectRatio)
{
  return glm::perspective(glm::radians(this->fov), aspectRatio, this->nearPlane, this->farPlane);
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