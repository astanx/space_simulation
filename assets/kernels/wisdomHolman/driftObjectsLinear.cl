#include "real.cl"

__kernel void driftObjectsLinear(__global real3* positions, __global real3* velocities, real dt)
{
  int id = get_global_id(0);
  positions[id] += velocities[id] * dt;
}