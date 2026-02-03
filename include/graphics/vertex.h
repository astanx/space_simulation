#pragma once

#include <glm/glm.hpp>

struct Vertex
{
  glm::vec3 position;
  glm::vec3 color;
  glm::vec2 texcoord;
  glm::vec3 normal;

  Vertex(
      glm::vec3 position = glm::vec3(0.f),
      glm::vec3 color = glm::vec3(0.f),
      glm::vec2 texcoord = glm::vec2(0.f),
      glm::vec3 normal = glm::vec3(0.f))
      : position(position), color(color), texcoord(texcoord), normal(normal) {};
};