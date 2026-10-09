#include "real.cl"

__kernel void driftLinear(__global real3* positions, __global real* mus, __global real3* velocities, int count, real dt)
{
  int id = get_global_id(0);
  if (id >= count) return;

  real3 acc = (real3)(0.0);

  real3 position = positions[id];

  for (int j = 0; j < count; j++)
  {
    if (id == j)
      continue;

    real3 dp = positions[j] - position;
    real distSq = dot(dp, dp);
    if (distSq < EPS)
      continue; // Avoid singularity

    real dist = sqrt(distSq);
    acc += dp * mus[j] / (dist * distSq);
  }

  velocities[id] += acc * dt;
  positions[id] += velocities[id] * dt;
}