#include "graphics/primitives/sphere.h"
#include "graphics/vertex.h"

#include <vector>
#include <cmath>

Sphere::Sphere(unsigned segments, float radius) : Primitive()
{
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  glm::vec3 topPos(0.f, radius, 0.f);
  vertices.push_back(Vertex{
      topPos,
      glm::vec3(1.f),
      glm::vec2(0.5f, 0.f),
      glm::normalize(topPos)});

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

      vertices.push_back(Vertex{
          pos,
          glm::vec3(1.f),
          glm::vec2(u, v),
          glm::normalize(pos)});
    }
  }

  glm::vec3 bottomPos(0.f, -radius, 0.f);
  vertices.push_back(Vertex{
      bottomPos,
      glm::vec3(1.f),
      glm::vec2(0.5f, 1.f),
      glm::normalize(bottomPos)});

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
