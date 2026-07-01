#include "graphics/primitives/point.h"

#include "graphics/vertex.h"

// Constructor and Destructor
Point::Point() : Primitive()
{
  this->positions.push_back(glm::vec3(0.f, 0.f, 0.f));
}