// #ifndef TORQUE_MATHS_H
// #define TORQUE_MATHS_H

// #ifdef __OPENCL_VERSION__
// #include "real.cl"
// #include "matrix.cl"
// typedef float3 vec3;
// #else
// #include <glm/glm.hpp>
// using dmat3 = glm::dmat3;
// using real3 = glm::dvec3;
// using real = double;
// #endif

// #include "maths/constants.h"

// inline real3 calculateGravitationalTorque(real3 dp, real d, dmat3 objectTensor, real bodyMu)
// {
//   if (d == 0.0)
//     return real3(0.0);

//   real3 r = dp / d;
// #ifdef __OPENCL_VERSION__
//   return 3 * bodyMu / pow(d, 3) * cross(r, dmat3_dot_d3(objectTensor, r));
// #else
//   return 3.0 * bodyMu / pow(d, 3) * glm::cross(r, objectTensor * r);
// #endif
// }

// inline real3 calculateTidalTorque(real3 dp, real d, real3 objectAngularVelocity,
//                                   real3 objectVelocity, real objectMeanRadius,
//                                   real objectLoveNumber, real objectTidalFactor,
//                                   real3 bodyVelocity, real bodyMu)
// {
//   if (d == 0.0)
//     return real3(0.0);

//   real3 v = objectVelocity - bodyVelocity;

//   real3 nVec = cross(dp, v) / dot(dp, dp);
//   real n = length(nVec);

//   if (n < EPS)
//     return real3(0.0);

//   return -3 * objectLoveNumber * 1 / (2 * n * objectTidalFactor) * bodyMu * bodyMu * pow(objectMeanRadius, 5) / G / pow(d, 6) * (objectAngularVelocity - nVec);
// }

// #endif

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
using glm::dot;
using glm::max;
#endif

#include "maths/constants.h"

inline real3 calculateGravitationalTorque(real3 dp, real d, dmat3 objectTensor, real bodyMu)
{
  if (!isfinite(d) || d == 0.0 || d < EPS)
#ifdef __OPENCL_VERSION__
    return (real3)(0.0);
#else
    return real3(0.0);
#endif

  real3 r = dp / d;
  real tensorScale = 0.0;
#ifdef __OPENCL_VERSION__
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      tensorScale = max(tensorScale, fabs(objectTensor.cols[i][j]));
#else
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      tensorScale = max(tensorScale, fabs(objectTensor[i][j]));
#endif

  if (tensorScale < EPS)
#ifdef __OPENCL_VERSION__
    return (real3)(0.0);
#else
    return real3(0.0);
#endif

  real3 crossRes;
#ifdef __OPENCL_VERSION__
  dmat3 tensor = dmat3_dot_d(objectTensor, 1 / tensorScale);
  crossRes = cross(r, dmat3_dot_d3(tensor, r));
#else
  dmat3 tensor = objectTensor / tensorScale;
  crossRes = glm::cross(r, tensor * r);
#endif

  real scale = 3.0 * bodyMu / d / d / d;
  real3 torque = crossRes * tensorScale * scale;

  if (!isfinite(torque.x) || !isfinite(torque.y) || !isfinite(torque.z))
#ifdef __OPENCL_VERSION__
    return (real3)(0.0);
#else
    return real3(0.0);
#endif

  return torque;
}

inline real3 calculateTidalTorque(real3 dp, real d, real3 objectAngularVelocity,
                                  real3 objectVelocity, real objectMeanRadius,
                                  real objectLoveNumber, real objectTidalFactor,
                                  real3 bodyVelocity, real bodyMu)
{
  if (dot(dp, dp) < EPS * EPS)
#ifdef __OPENCL_VERSION__
    return (real3)(0.0);
#else
    return real3(0.0);
#endif

  if (!isfinite(d) || d < EPS)
#ifdef __OPENCL_VERSION__
    return (real3)(0.0);
#else
    return real3(0.0);
#endif

  if (objectLoveNumber == -1 && objectTidalFactor == -1)
#ifdef __OPENCL_VERSION__
    return (real3)(0.0);
#else
    return real3(0.0);
#endif

  real3 v = objectVelocity - bodyVelocity;

  real3 nVec = cross(dp, v) / dot(dp, dp);
  real n = length(nVec);

  if (n < EPS)
#ifdef __OPENCL_VERSION__
    return (real3)(0.0);
#else
    return real3(0.0);
#endif

  real radiusFraction = objectMeanRadius / d;
  real scale = bodyMu * radiusFraction * radiusFraction * radiusFraction;
  scale *= -3 * objectLoveNumber * 1 / (2 * n * objectTidalFactor) / d;
  scale *= bodyMu * radiusFraction * radiusFraction;
  scale /= G;

  real3 torque = scale * (objectAngularVelocity - nVec);

  if (!isfinite(torque.x) || !isfinite(torque.y) || !isfinite(torque.z))
#ifdef __OPENCL_VERSION__
  return (real3)(0.0);
#else
    return real3(0.0);
#endif
  return torque;
}

#endif