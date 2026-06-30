#include "graphics/primitives/point.h"

#include "graphics/vertex.h"

// Constructor and Destructor
Point::Point() : Primitive()
{
  std::vector<Vertex> vertices =
      {
          {glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)},
      };

  std::vector<GLuint> indices = {};

  this->set(vertices, indices);
}