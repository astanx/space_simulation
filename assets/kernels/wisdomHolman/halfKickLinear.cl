#include "constants.cl"

__kernel void halfKickLinear(__global double3* positions, __global double* mus, __global double3* velocities, __global double3* accelerations, __global int* centralBodyIndices, int count, double dt)
{
  int id = get_global_id(0);
  if (id >= count) return;

  accelerations[id] = double3(0.0);

  int centralBodyID = centralBodyIndices[id];

  double3 position = positions[id];

  for (int j = 0; j < count; j++)
  {
    if (id == j)
      continue;
    if (id == centralBodyID)
      continue;

    double3 dp = positions[j] - position;
    double distSq = dot(dp, dp);
    if (distSq < EPS)
      return; // Avoid singularity

    double dist = sqrt(distSq);
    accelerations[id] += dp * mus[j] / (dist * distSq);
  }

  velocities[id] += accelerations[id] * dt;
}