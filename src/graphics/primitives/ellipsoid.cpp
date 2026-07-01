#include "graphics/primitives/ellipsoid.h"

#include "graphics/vertex.h"

#include "physics/structs/radii.h"

#include <vector>
#include <cmath>

// Constructor
Ellipsoid::Ellipsoid(unsigned segments, Radii radii, bool tangent) : Primitive()
{
  glm::vec3 topPos(0.f, radii.polar, 0.f);
  glm::vec3 topNormal(0.f, 1 / radii.polar, 0.f);

  this->positions.push_back(topPos);
  this->normals.push_back(glm::normalize(topNormal));
  this->texcoords.push_back(glm::vec2(0.5f, 0.f));

  for (unsigned i = 1; i < segments; ++i)
  {
    float v = float(i) / float(segments);
    float phi = v * M_PI;

    for (unsigned j = 0; j <= segments; ++j)
    {
      float u = float(j) / float(segments);
      float theta = u * 2.f * M_PI;

      glm::vec3 pos(
          radii.equatorian * sin(phi) * cos(theta),
          radii.polar * cos(phi),
          radii.equatorian * sin(phi) * sin(theta));

      glm::vec3 normal(
          pos.x / (radii.equatorian * radii.equatorian),
          pos.y / (radii.polar * radii.polar),
          pos.z / (radii.equatorian * radii.equatorian));
      normal = glm::normalize(normal);

      this->positions.push_back(pos);
      this->normals.push_back(normal);
      this->texcoords.push_back(glm::vec2(u, v));
    }
  }

  glm::vec3 bottomPos(0.f, -radii.polar, 0.f);
  glm::vec3 bottomNormal(0.f, -1 / radii.polar, 0.f);
  this->positions.push_back(bottomPos);
  this->normals.push_back(glm::normalize(bottomNormal));
  this->texcoords.push_back(glm::vec2(0.5f, 1.f));

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

  if (tangent)
    this->computeTangents();
}
