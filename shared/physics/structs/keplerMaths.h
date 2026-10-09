#ifndef KEPLER_MATHS_H
#define KEPLER_MATHS_H

#ifdef __OPENCL_VERSION__
#include "real.cl"
#else
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>

using glm::clamp;
using glm::cross;
using glm::dot;
using glm::length;
using std::acos;
using std::atan2;
using std::cos;
using std::fabs;
using std::fmod;
using std::sin;
using std::sqrt;
#endif

#include "maths/constants.h"
#include "physics/structs/keplerElements.h"

#ifndef __OPENCL_VERSION__
template <typename real>
#endif
inline real calculateMeanMotion(real mu, real a)
{
  return sqrt(mu / (a * a * a));
}

#ifndef __OPENCL_VERSION__
template <typename real>
#endif
inline real advanceMeanAnomaly(real m, real n, real dt)
{
  m = fmod(m + n * dt, 2 * PI);

  if (m < 0)
    m += 2 * PI;

  return m;
}
#ifndef __OPENCL_VERSION__
template <typename real>
inline KeplerElements<real> calculateKeplerElements(
    real mu,
    glm::vec<3, real> position,
    glm::vec<3, real> velocity)
#else
inline KeplerElements calculateKeplerElements(
    real mu,
    real3 position,
    real3 velocity)
#endif
{
#ifndef __OPENCL_VERSION__
  using real3 = glm::vec<3, real>;
  KeplerElements<real> elements;
#else
  KeplerElements elements;
#endif
  elements.a = 0;
  elements.e = 0;
  elements.i = 0;
  elements.Omega = 0;
  elements.omega = 0;
  elements.m = 0;
  elements.n = 0;

  if (!(mu > 0))
    return elements;

#ifndef __OPENCL_VERSION__
  if (!std::isfinite(mu))
    return elements;
#else
  if (!isfinite(mu))
    return elements;
#endif

  real posLen = length(position);
  real vSq = dot(velocity, velocity);

#ifndef __OPENCL_VERSION__
  if (!std::isfinite(posLen) ||
      !std::isfinite(vSq))
    return elements;
#else
  if (!isfinite(posLen) ||
      !isfinite(vSq))
    return elements;
#endif

  if (!(posLen > 0) || !(vSq >= 0))
    return elements;

  real3 hVec = cross(position, velocity);
  real h = length(hVec);

#ifndef __OPENCL_VERSION__
  if (!std::isfinite(h))
    return elements;
#else
  if (!isfinite(h))
    return elements;
#endif

  if (!(h > EPS * posLen * sqrt(vSq)))
    return elements;

  real3 hNorm = hVec / h;

  real3 eVec =
      cross(velocity, hVec) / mu - position / posLen;

  real e = length(eVec);

#ifndef __OPENCL_VERSION__
  if (!std::isfinite(e))
    return elements;
#else
  if (!isfinite(e))
    return elements;
#endif

  if (e <= EPS)
    e = 0;

  real energy = 0.5 * vSq - mu / posLen;

#ifndef __OPENCL_VERSION__
  if (!std::isfinite(energy))
    return elements;
#else
  if (!isfinite(energy))
    return elements;
#endif

  real a = 0;

  if (energy != 0)
  {
    a = -mu / (2 * energy);

#ifndef __OPENCL_VERSION__
    if (!std::isfinite(a))
      a = 0;
#else
    if (!isfinite(a))
      a = 0;
#endif
  }

  real i = atan2(
      sqrt(hVec.x * hVec.x + hVec.y * hVec.y),
      hVec.z);

#ifndef __OPENCL_VERSION__
  real3 k(0, 0, 1);
#else
  real3 k = (real3)(0, 0, 1);
#endif

  real3 nVec = cross(k, hVec);
  real nodeLength = length(nVec);

  bool hasNode = nodeLength > EPS * h;

  real Omega = 0;

  if (hasNode)
  {
    Omega = atan2(nVec.y, nVec.x);
  }

  real omega = 0;
  real nu = 0;

  if (e > 0)
  {
    if (hasNode)
    {
      omega = atan2(
          dot(hNorm, cross(nVec, eVec)),
          dot(nVec, eVec));
    }
    else
    {
      real direction = hVec.z >= 0 ? 1 : -1;

      omega = atan2(
          direction * eVec.y,
          eVec.x);
    }

    nu = atan2(
        dot(hNorm, cross(eVec, position)),
        dot(eVec, position));
  }
  else
  {
    if (hasNode)
    {
      nu = atan2(
          dot(hNorm, cross(nVec, position)),
          dot(nVec, position));
    }
    else
    {
      real direction = hVec.z >= 0 ? 1 : -1;

      nu = atan2(
          direction * (position).y,
          (position).x);
    }
  }

  if (Omega < 0)
    Omega += 2 * PI;

  if (omega < 0)
    omega += 2 * PI;

  if (nu < 0)
    nu += 2 * PI;

  elements.a = a;
  elements.e = e;
  elements.i = i;
  elements.Omega = Omega;
  elements.omega = omega;

  if (!(energy < 0) || !(e < 1) || !(a > 0))
    return elements;

  real E = 2 * atan2(
                   sqrt(1 - e) * sin(nu * 0.5),
                   sqrt(1 + e) * cos(nu * 0.5));

  real M = E - e * sin(E);

  M = fmod(M, 2 * PI);

  if (M < 0)
    M += 2 * PI;

  elements.m = M;

#ifndef __OPENCL_VERSION__
  elements.n = calculateMeanMotion<real>(mu, a);
#else
  elements.n = calculateMeanMotion(mu, a);
#endif

  return elements;
}
#endif