#ifndef QUATERNION
#define QUATERNION

#include "real.cl"

typedef real4 quat;


quat quat_dot_quat(quat q1, quat q2)
{
  real w1 = q1.x;
  real3 v1 = q1.yzw;

  real w2 = q2.x;
  real3 v2 = q2.yzw;

  real w = w1 * w2 - dot(v1, v2);
  real3 v = w1 * v2 + w2 * v1 + cross(v1, v2);

  return (quat)(w, v.x, v.y, v.z);
}

#endif