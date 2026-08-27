#ifndef KEPLER_MATHS_H
#define KEPLER_MATHS_H

#ifdef __OPENCL_VERSION__
#include "real.cl"
#else
#include <cmath>
using std::fmod;
using std::sqrt;
#endif

#include "maths/constants.h"

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

#endif