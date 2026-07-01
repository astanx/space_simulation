#pragma once

#include <glm/glm.hpp>

class OrientationSource
{
protected:
  glm::dmat3 orientation;

public:
  virtual ~OrientationSource() = default;
  const glm::dmat3 &getOrientation() const { return this->orientation; };
  void setOrientation(const glm::dmat3 &orientation) { this->orientation = orientation; };
};