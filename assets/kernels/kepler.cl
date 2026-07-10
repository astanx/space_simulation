#ifndef KEPLER
#define KEPLER

#include "constants.cl"
#include "real.cl"

real calculateMeanMotion(real mu, real a)
{
  return sqrt(mu / (a * a * a));
}

real advanceMeanAnomaly(real m, real n, real dt)
{
  m = fmod(m + n * dt, 2 * PI);
  
  if (m < 0)
    m += 2 * PI;

  return m;
}

#endif