#include "maths/constants.h"
#include "matrix.cl"
#include "real.cl"
#include "quaternion.cl"

__kernel void driftAngular(__global real3* angularVelocities, __global dquat* orientations, real dt)
{
  int id = get_global_id(0);
  real3 omega = angularVelocities[id];

  real omega_len = length(omega);
  real theta = omega_len * dt;
  if (fabs(theta) > EPS)
  {
    real3 axis = omega / omega_len;
    real half_theta = theta * 0.5;
    dquat q_rot = (dquat)(sin(half_theta) * axis.x, sin(half_theta) * axis.y, sin(half_theta) * axis.z, cos(half_theta));

    orientations[id] = dquat_normalize(dquat_dot_dquat(q_rot, orientations[id]));
  }
}