#include "graphics/primitives/ellipsoid.h"
#include "graphics/vertex.h"
#include "physics/structs/radii.h"

#include <vector>
#include <cmath>

Ellipsoid::Ellipsoid(unsigned segments, Radii radii) : Primitive()
{
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  glm::vec3 topPos(0.f, radii.polar, 0.f);
  glm::vec3 topNormal(0.f, 1 / radii.polar, 0.f);
  vertices.push_back(Vertex{
      topPos,
      glm::vec3(1.f),
      glm::vec2(0.5f, 0.f),
      glm::normalize(topNormal)});

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

      vertices.push_back(Vertex{
          pos,
          glm::vec3(1.f),
          glm::vec2(u, v),
          normal});
    }
  }

  glm::vec3 bottomPos(0.f, -radii.polar, 0.f);
  glm::vec3 bottomNormal(0.f, -1 / radii.polar, 0.f);
  vertices.push_back(Vertex{
      bottomPos,
      glm::vec3(1.f),
      glm::vec2(0.5f, 1.f),
      glm::normalize(bottomNormal)});

  unsigned ringVertices = segments + 1;
  unsigned top = 0;
  unsigned bottom = vertices.size() - 1;

  for (unsigned j = 0; j < segments; ++j)
  {
    indices.push_back(top);
    indices.push_back(1 + j + 1);
    indices.push_back(1 + j);
  }

  for (unsigned i = 0; i < segments - 2; ++i)
  {
    for (unsigned j = 0; j < segments; ++j)
    {
      unsigned first = 1 + i * ringVertices + j;
      unsigned second = first + ringVertices;

      indices.push_back(first);
      indices.push_back(first + 1);
      indices.push_back(second);

      indices.push_back(second);
      indices.push_back(first + 1);
      indices.push_back(second + 1);
    }
  }

  unsigned base = bottom - ringVertices;
  for (unsigned j = 0; j < segments; ++j)
  {
    indices.push_back(bottom);
    indices.push_back(base + j);
    indices.push_back(base + j + 1);
  }

  this->set(vertices.data(), vertices.size(), indices.data(), indices.size());
}
