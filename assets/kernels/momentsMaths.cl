#ifndef MOMENTS_MATHS
#define MOMENTS_MATHS

#include "matrix.cl"
#include "maths/constants.h"
#include "real.cl"

#include "maths/torqueMaths.h"

typedef struct
{
  real3 position;
  dmat3 tensor;
  real3 angularVelocity;
  real3 velocity;
  real meanRadius;
} ObjectState;

typedef struct
{
  real loveNumber;
  real tidalFactor;
  int isTidal;
} TidalProperties;

real3 calculateTorque(ObjectState object, TidalProperties properties, 
                        __global real3* positions, __global real3* velocities, __global real* mus, 
                        int i, int count)
{
  real3 torque = (real3)(0.0, 0.0, 0.0);

  for (int j = 0; j < count; j++)
  {
    if (i == j)
      continue;

    real3 bodyPosition = positions[j];
    real bodyMu = mus[j];

    real3 dp = object.position - bodyPosition;
    real d = length(dp);

    if (!isfinite(d) || d < EPS) continue;

    torque += calculateGravitationalTorque(dp, d, object.tensor, bodyMu);
    if (properties.isTidal)
      torque += calculateTidalTorque(dp, d, object.angularVelocity, object.velocity, 
                                      object.meanRadius, properties.loveNumber, properties.tidalFactor, 
                                      velocities[j], bodyMu);
  }
  return torque;
}

#endif