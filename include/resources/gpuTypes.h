#pragma once

#include <glm/glm.hpp>

template <typename Real>
using Vec3 = glm::vec<3, Real>;

template <typename Real>
using Mat3 = glm::mat<3, 3, Real>;

template <typename Real>
using Quat = glm::qua<Real>;