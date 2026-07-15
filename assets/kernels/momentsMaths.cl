#ifndef MOMENTS_MATHS
#define MOMENTS_MATHS

#include "matrix.cl"
#include "constants.cl"
#include "real.cl"

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

real3 calculateGravitationalTorque(real3 dp, real d, dmat3 objectTensor, real bodyMu)
{
  if (d == 0.0)
    return real3(0.0);

  real3 r = dp / d;
  return 3 *  bodyMu / pow(d, 3) * cross(r, dmat3_dot_d3(objectTensor, r));
}

real3 calculateTidalTorque(real3 dp, real d, real3 objectAngularVelocity, 
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

real3 calculateTorque(ObjectState object, TidalProperties properties, 
                        __global real3* positions, __global real3* velocities, __global real* mus, 
                        int i, int count)
{
  real3 torque = (real3)(0.0);

  for (int j = 0; j < count; j++)
  {
    if (i == j)
      continue;

    real3 bodyPosition = positions[j];
    real bodyMu = mus[j];

    real3 dp = object.position - bodyPosition;
    real d = length(dp);

    if (d == 0.0) continue;

    torque += calculateGravitationalTorque(dp, d, object.tensor, bodyMu);
    if (properties.isTidal)
      torque += calculateTidalTorque(dp, d, object.angularVelocity, object.velocity, 
                                      object.meanRadius, properties.loveNumber, properties.tidalFactor, 
                                      velocities[j], bodyMu);
  }
  return torque;
}

#endif