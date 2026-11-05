#include "maths/constants.h"
#include "real.cl"

#include "physics/calculateGravitationalAcceleration.h"

__kernel void halfKickObjectsLinear(__global real3* positions, __global real* mus, __global real3* velocities, __global int* centralBodyIndices, int count, real dt)
{
  int id = get_global_id(0);
  if (id >= count) return;

  real3 acc = (real3)(0.0);

  int centralBodyID = centralBodyIndices[id];

  real3 position = positions[id];

  for (int j = 0; j < count; j++)
  {
    if (id == j)
      continue;

    real3 otherPosition = positions[j];
    
    real3 scale = gravitationalDpOverD3(position, otherPosition);

    if (j != centralBodyID)
      acc += scale * mus[j];
  }

  velocities[id] += acc * dt;
}