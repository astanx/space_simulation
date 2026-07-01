#pragma once

#include "debug/logger.h"

#include <vector>

template <VertexInitable T>
std::vector<T> Primitive::getVertices()
{
  std::vector<T> vertices;

  vertices.resize(this->positions.size());

  for (size_t i = 0; i < this->positions.size(); i++)
  {
    vertices[i].init(this->positions[i],
                     !this->texcoords.empty() ? this->texcoords[i] : glm::vec2(0.0f),
                     !this->normals.empty() ? this->normals[i] : glm::vec3(0.0f),
                     !this->colors.empty() ? this->colors[i] : glm::vec3(0.0f),
                     !this->tangents.empty() ? glm::vec4(this->tangents[i], 1.0f) : glm::vec4(0.0f));
  }
  return vertices;
}
