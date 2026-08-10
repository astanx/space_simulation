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
  if (theta > EPS)
  {
    real3 axis = omega / omega_len;
    real half_theta = theta * 0.5;
    dquat q_rot = (dquat)(sin(half_theta) * axis.x, sin(half_theta) * axis.y, sin(half_theta) * axis.z, cos(half_theta));

    orientations[id] = dquat_normalize(dquat_dot_dquat(orientations[id], q_rot));
/*
    dmat3 skew;
    skew.cols[0] = (real3)(0, omega.z, -omega.y);
    skew.cols[1] = (real3)(-omega.z, 0, omega.x);
    skew.cols[2] = (real3)(omega.y, -omega.x, 0);

    skew = dmat3_dot_d(skew, dt);

    dmat3 skewSq = dmat3_dot_dmat3(skew, skew);
    dmat3 exp = dmat3_plus_dmat3(dmat3_dot_d(skew, sin(theta) / theta), dmat3_dot_d(skewSq, (1 - cos(theta)) / theta / theta));
    exp = dmat3_plus_dmat3(exp, dmat3_identity());

    orientations[id] = dmat3_dot_dmat3(orientations[id], exp);
    */
  }
}