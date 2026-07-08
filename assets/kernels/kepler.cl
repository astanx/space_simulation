#ifndef KEPLER
#define KEPLER

#include "constants.cl"

double calculateMeanMotion(double mu, double a)
{
  return sqrt(mu / (a * a * a));
}

double advanceMeanAnomaly(double m, double n, double dt)
{
  m = fmod(m + n * dt, 2 * PI);
  
  if (m < 0)
    m += 2 * PI;

  return m;
}

#endif