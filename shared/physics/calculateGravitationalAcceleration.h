#ifndef CALCULATE_GRAVITATIONAL_ACCELERATION_H
#define CALCULATE_GRAVITATIONAL_ACCELERATION_H

#ifdef __OPENCL_VERSION__
#include "real.cl"
using gravity_vec3 = real3;
#else
#include <cmath>
#include <glm/glm.hpp>
using glm::dot;
using std::sqrt;
#endif

#include "maths/constants.h"
#ifdef __OPENCL_VERSION__
inline gravity_vec3 gravitationalDpOverD3(gravity_vec3 thisPosition, gravity_vec3 otherPosition)
#else
template <typename real>
inline glm::vec<3, real> gravitationalDpOverD3(const glm::vec<3, real> &thisPosition, const glm::vec<3, real> &otherPosition)
#endif
{
#ifndef __OPENCL_VERSION__
  using gravity_vec3 = glm::vec<3, real>;
#endif
  gravity_vec3 dp = otherPosition - thisPosition;
  real distSq = dot(dp, dp);

  if (distSq < EPS)
    return gravity_vec3(0.0);

  real dist = sqrt(distSq);

  return dp / dist / distSq;
}

#endif