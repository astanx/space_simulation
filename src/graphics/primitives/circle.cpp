#include "graphics/primitives/circle.h"
#include "graphics/vertex.h"

#include <vector>
#include <cmath>

// Constructor and Destructor
Circle::Circle(unsigned segments, float radius) : Primitive()
{
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  vertices.push_back(Vertex{glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(1.0f, 1.0f, 1.0f),
                            glm::vec2(0.5f, 0.5f),
                            glm::vec3(0.f, 0.f, 1.f)});

  for (unsigned i = 0; i <= segments; i++)
  {
    float angle = 2.f * M_PI / segments * float(i);
    float x = radius * cos(angle);
    float y = radius * sin(angle);
    vertices.push_back(Vertex{glm::vec3(x, y, 0.f), glm::vec3(1.0f, 1.0f, 1.0f),
                              glm::vec2((x / radius + 1.0f) * 0.5f, (y / radius + 1.0f) * 0.5f),
                              glm::vec3(0.f, 0.f, 1.f)});
  }

  for (unsigned i = 1; i <= segments; i++)
  {
    indices.push_back(0);
    indices.push_back(i);
    indices.push_back(i + 1);
  }
  
  indices[indices.size() - 1] = 1;

  this->set(vertices.data(), vertices.size(), indices.data(), indices.size());
}