#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// View space
struct Transform
{
  glm::vec3 position;
  glm::quat orientation;
};