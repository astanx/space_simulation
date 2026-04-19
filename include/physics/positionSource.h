#pragma once

#include <glm/glm.hpp>

class PositionSource
{
public:
  virtual ~PositionSource() = default;
  virtual glm::dvec3 getPosition() const = 0;
};