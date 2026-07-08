__kernel void driftObjectsLinear(__global double3* positions, __global double3* velocities, double dt)
{
  int id = get_global_id(0);
  positions[id] += velocities[id] * dt;
}