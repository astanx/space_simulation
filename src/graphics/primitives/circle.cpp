#include "graphics/primitives/circle.h"

#include "graphics/vertex.h"

#include <vector>
#include <cmath>

// Constructor and Destructor
Circle::Circle(unsigned segments, float radius) : Primitive()
{
  this->positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(0.5f, 0.5f));
  this->normals.push_back(glm::vec3(0.f, 0.f, 1.f));

  for (unsigned i = 0; i <= segments; i++)
  {
    float angle = 2.f * M_PI / segments * float(i);
    float x = radius * cos(angle);
    float y = radius * sin(angle);
    this->positions.push_back(glm::vec3(x, y, 0.f));
    this->texcoords.push_back(glm::vec2((x / radius + 1.0f) * 0.5f, (y / radius + 1.0f) * 0.5f));
    this->normals.push_back(glm::vec3(0.f, 0.f, 1.f));
  }

  for (unsigned i = 1; i <= segments; i++)
  {
    this->indices.push_back(0);
    this->indices.push_back(i);
    this->indices.push_back(i + 1);
  }

  this->indices[this->indices.size() - 1] = 1;
}