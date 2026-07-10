#include "constants.cl"
#include "real.cl"

__kernel void halfKickLinear(__global real3* positions, __global real* mus, __global real3* velocities, __global real3* accelerations, __global int* centralBodyIndices, int count, real dt)
{
  int id = get_global_id(0);
  if (id >= count) return;

  accelerations[id] = real3(0.0);

  int centralBodyID = centralBodyIndices[id];

  real3 position = positions[id];

  for (int j = 0; j < count; j++)
  {
    if (id == j)
      continue;
    if (id == centralBodyID)
      continue;

    real3 dp = positions[j] - position;
    real distSq = dot(dp, dp);
    if (distSq < EPS)
      return; // Avoid singularity

    real dist = sqrt(distSq);
    accelerations[id] += dp * mus[j] / (dist * distSq);
  }

  velocities[id] += accelerations[id] * dt;
}