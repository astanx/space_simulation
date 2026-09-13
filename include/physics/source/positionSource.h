#pragma once

#include <glm/glm.hpp>

class PositionSource
{
protected:
  glm::dvec3 position;

public:
  virtual ~PositionSource() = default;
  const glm::dvec3 &getPosition() const { return this->position; };
  void setPosition(const glm::dvec3 &position) { this->position = position; };
};