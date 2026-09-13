#pragma once

#include <glm/gtc/quaternion.hpp>

class OrientationSource
{
protected:
  glm::dquat orientation;

public:
  virtual ~OrientationSource() = default;
  const glm::dquat &getOrientation() const { return this->orientation; };
  void setOrientation(const glm::dquat &orientation) { this->orientation = orientation; };
};