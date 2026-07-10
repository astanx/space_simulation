#include "constants.cl"
#include "matrix.cl"

__kernel void driftObjectsLinear(__global double3* angularVelocities, __global double4* velocities, __global dmat3* orientations, double dt)
{
  int id = get_global_id(0);
  double3 omega = angularVelocities[id];

  double theta = length(omega) * dt;
  if (theta > EPS)
  {
    dmat3 skew;
    skew.cols[0] = double3(0, omega.z, -omega.y);
    skew.cols[1] = double3(-omega.z, 0, omega.x);
    skew.cols[2] = double3(omega.y, -omega.x, 0);

    skew *= dt;

    dmat3 mat;
    dmat3 exp = identity() + sin(theta) * skew / theta + (1 - cos(theta)) / theta / theta * skew * skew;

    orientations[id] *= exp;
  }
}