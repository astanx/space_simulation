#include "constants.cl"
#include "matrix.cl"
#include "real.cl"

__kernel void driftAngular(__global real3* angularVelocities, __global dmat3* orientations, real dt)
{
  int id = get_global_id(0);
  real3 omega = angularVelocities[id];

  real theta = length(omega) * dt;
  if (theta > EPS)
  {
    dmat3 skew;
    skew.cols[0] = (real3)(0, omega.z, -omega.y);
    skew.cols[1] = (real3)(-omega.z, 0, omega.x);
    skew.cols[2] = (real3)(omega.y, -omega.x, 0);

    skew = dmat3_dot_d(skew, dt);

    dmat3 skewSq = dmat3_dot_dmat3(skew, skew);
    dmat3 exp = dmat3_plus_dmat3(dmat3_dot_d(skew, sin(theta) / theta), dmat3_dot_d(skewSq, (1 - cos(theta)) / theta / theta));
    exp = dmat3_plus_dmat3(exp, dmat3_identity());

    orientations[id] = dmat3_dot_dmat3(orientations[id], exp);
  }
}