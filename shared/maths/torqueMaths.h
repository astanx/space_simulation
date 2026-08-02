#ifndef TORQUE_MATHS_H
#define TORQUE_MATHS_H

#ifdef __OPENCL_VERSION__
#include "real.cl"
#include "matrix.cl"
typedef float3 vec3;
#else
#include <glm/glm.hpp>
using dmat3 = glm::dmat3;
using real3 = glm::dvec3;
using real = double;
#endif

#include "maths/constants.h"

inline real3 calculateGravitationalTorque(real3 dp, real d, dmat3 objectTensor, real bodyMu)
{
  if (d == 0.0)
    return real3(0.0);

  real3 r = dp / d;
#ifdef __OPENCL_VERSION__
  return 3 * bodyMu / pow(d, 3) * cross(r, dmat3_dot_d3(objectTensor, r));
#else
  return 3.0 * bodyMu / pow(d, 3) * glm::cross(r, objectTensor * r);
#endif
}

inline real3 calculateTidalTorque(real3 dp, real d, real3 objectAngularVelocity,
                                  real3 objectVelocity, real objectMeanRadius,
                                  real objectLoveNumber, real objectTidalFactor,
                                  real3 bodyVelocity, real bodyMu)
{
  if (d == 0.0)
    return real3(0.0);

  real3 v = objectVelocity - bodyVelocity;

  real3 nVec = cross(dp, v) / dot(dp, dp);
  real n = length(nVec);

  if (n < EPS)
    return real3(0.0);

  return -3 * objectLoveNumber * 1 / (2 * n * objectTidalFactor) * bodyMu * bodyMu * pow(objectMeanRadius, 5) / G / pow(d, 6) * (objectAngularVelocity - nVec);
}

#endif