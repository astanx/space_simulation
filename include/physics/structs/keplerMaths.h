#ifndef KEPLER_MATHS_H
#define KEPLER_MATHS_H

#ifdef __OPENCL_VERSION__
#include "real.cl"
#else
#include <cmath>
using real = double;
using std::sqrt;
using std::fmod;
#endif

#include "maths/constants.h"

inline real calculateMeanMotion(real mu, real a)
{
  return sqrt(mu / (a * a * a));
}

inline real advanceMeanAnomaly(real m, real n, real dt)
{
  m = fmod(m + n * dt, 2 * PI);
  
  if (m < 0)
    m += 2 * PI;

  return m;
}

#endif