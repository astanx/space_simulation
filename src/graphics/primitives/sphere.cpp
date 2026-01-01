#include "graphics/primitives/sphere.h"
#include "graphics/vertex.h"

#include <vector>
#include <cmath>

// Constructor and Destructor
Sphere::Sphere(unsigned segments, float radius) : Primitive()
{
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  vertices.push_back(Vertex{glm::vec3(0.f, radius, 0.f),
                            glm::vec3(1.f),
                            glm::vec2(0.5f, 0.f),
                            glm::vec3(0.f, 1.f, 0.f)});

  for (unsigned i = 1; i <= segments; i++)
  {
    float phi = M_PI * float(i) / float(segments);
    for (unsigned j = 0; j <= segments; j++)
    {
      float theta = 2.f * M_PI * float(j) / float(segments);

      float x = radius * sin(phi) * cos(theta);
      float y = radius * cos(phi);
      float z = radius * sin(phi) * sin(theta);

      glm::vec3 pos(x, y, z);
      glm::vec3 normal = glm::normalize(pos);
      glm::vec2 texcoord(theta / (2.f * M_PI), phi / M_PI);

      vertices.push_back(Vertex{pos, glm::vec3(1.f), texcoord, normal});
    }
  }

  unsigned topIndex = 0;
  for (unsigned j = 0; j < segments; j++)
  {
    indices.push_back(topIndex);
    indices.push_back(1 + j);
    indices.push_back(1 + j + 1);
  }

  for (unsigned i = 0; i < segments - 2; i++)
  {
    for (unsigned j = 0; j < segments; j++)
    {
      unsigned first = 1 + i * (segments + 1) + j;
      unsigned second = first + segments + 1;

      indices.push_back(first);
      indices.push_back(second);
      indices.push_back(first + 1);

      indices.push_back(second);
      indices.push_back(second + 1);
      indices.push_back(first + 1);
    }
  }

  unsigned bottomIndex = vertices.size() - 1;
  unsigned base = bottomIndex - (segments + 1);
  for (unsigned j = 0; j < segments; j++)
  {
    indices.push_back(bottomIndex);
    indices.push_back(base + j + 1);
    indices.push_back(base + j);
  }

  this->set(vertices.data(), vertices.size(), indices.data(), indices.size());
}