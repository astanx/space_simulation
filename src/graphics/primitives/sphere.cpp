#include "graphics/primitives/sphere.h"

#include "graphics/vertex.h"

#include <vector>
#include <cmath>

Sphere::Sphere(unsigned segments, float radius) : Primitive()
{
  glm::vec3 topPos(0.f, radius, 0.f);
  this->positions.push_back(topPos);
  this->texcoords.push_back(glm::vec2(0.5f, 0.f));
  this->normals.push_back(glm::normalize(topPos));

  for (unsigned i = 1; i < segments; ++i)
  {
    float v = float(i) / float(segments);
    float phi = v * M_PI;

    for (unsigned j = 0; j <= segments; ++j)
    {
      float u = float(j) / float(segments);
      float theta = u * 2.f * M_PI;

      glm::vec3 pos(
          radius * sin(phi) * cos(theta),
          radius * cos(phi),
          radius * sin(phi) * sin(theta));

      this->positions.push_back(pos);
      this->texcoords.push_back(glm::vec2(u, v));
      this->normals.push_back(glm::normalize(pos));
    }
  }

  glm::vec3 bottomPos(0.f, -radius, 0.f);
  this->positions.push_back(bottomPos);
  this->texcoords.push_back(glm::vec2(0.5f, 1.f));
  this->normals.push_back(glm::normalize(bottomPos));

  unsigned ringVertices = segments + 1;
  unsigned top = 0;
  unsigned bottom = this->positions.size() - 1;

  for (unsigned j = 0; j < segments; ++j)
  {
    this->indices.push_back(top);
    this->indices.push_back(1 + j + 1);
    this->indices.push_back(1 + j);
  }

  for (unsigned i = 0; i < segments - 2; ++i)
  {
    for (unsigned j = 0; j < segments; ++j)
    {
      unsigned first = 1 + i * ringVertices + j;
      unsigned second = first + ringVertices;

      this->indices.push_back(first);
      this->indices.push_back(first + 1);
      this->indices.push_back(second);

      this->indices.push_back(second);
      this->indices.push_back(first + 1);
      this->indices.push_back(second + 1);
    }
  }

  unsigned base = bottom - ringVertices;
  for (unsigned j = 0; j < segments; ++j)
  {
    this->indices.push_back(bottom);
    this->indices.push_back(base + j);
    this->indices.push_back(base + j + 1);
  }
}
