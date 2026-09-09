#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

template <typename Real>
struct RealTypes
{
  using vec3 = glm::vec<3, Real>;
  using mat3 = glm::mat<3, 3, Real>;
  using quat = glm::qua<Real>;
};