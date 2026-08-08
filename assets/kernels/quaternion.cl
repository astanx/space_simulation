#ifndef QUATERNION
#define QUATERNION

#include "real.cl"
#include "matrix.cl"

typedef real4 dquat;
typedef float4 quat;

float quat_length(quat q)
{
  return sqrt(dot(q.xyz, q.xyz) + q.w * q.w);
}

real dquat_length(dquat q)
{
  return sqrt(dot(q.xyz, q.xyz) + q.w * q.w);
}

quat quat_normalize(quat q)
{
  float len = quat_length(q);
  if (len < 1e-12)
    return (quat)(0, 0, 0, 1);

  return q / len;
}

dquat dquat_normalize(dquat q)
{
  real len = dquat_length(q);
  if (len < 1e-12)
    return (dquat)(0, 0, 0, 1);

  return q / len;
}

dquat dquat_dot_dquat(dquat q1, dquat q2)
{
  real w1 = q1.w;
  real3 v1 = q1.xyz;

  real w2 = q2.w;
  real3 v2 = q2.xyz;

  real w = w1 * w2 - dot(v1, v2);
  real3 v = w1 * v2 + w2 * v1 + cross(v1, v2);

  return (dquat)(v.x, v.y, v.z, w);
}

quat quat_dot_quat(quat q1, quat q2)
{
  float w1 = q1.w;
  float3 v1 = q1.xyz;

  float w2 = q2.w;
  float3 v2 = q2.xyz;

  float w = w1 * w2 - dot(v1, v2);
  float3 v = w1 * v2 + w2 * v1 + cross(v1, v2);

  return (quat)(v.x, v.y, v.z, w);
}

dmat3 dquat_to_dmat3(dquat q)
{
  dmat3 m;

  m.cols[0] = (real3)(
    1 - 2 * q.y * q.y - 2 * q.z * q.z,
    2 * q.x * q.y + 2 * q.w * q.z,
    2 * q.x * q.z - 2 * q.w * q.y
  );

  m.cols[1] = (real3)(
    2 * q.x * q.y - 2 * q.w * q.z,
    1 - 2 * q.x * q.x - 2 * q.z * q.z,
    2 * q.y * q.z + 2 * q.w * q.x
  );

  m.cols[2] = (real3)(
    2 * q.x * q.z + 2 * q.w * q.y,
    2 * q.y * q.z - 2 * q.w * q.x,
    1 - 2 * q.x * q.x - 2 * q.y * q.y
  );

  return m;
}

dquat dmat3_to_dquat(dmat3 m)
{
  real m00 = m.cols[0].x;
  real m11 = m.cols[1].y;
  real m22 = m.cols[2].z;
  real t = m00 + m11 + m22;

  dquat q = (dquat)(0, 0, 0, 1);
  if (t > 0)
  {
    q.w = 0.5 * sqrt(t + 1);
    q.x = (m.cols[1].z - m.cols[2].y) / 4 / q.w;
    q.y = (m.cols[2].x - m.cols[0].z) / 4 / q.w;;
    q.z = (m.cols[1].x - m.cols[0].y) / 4 / q.w;;
  }

  else if (m00 > m11 && m00 > m22)
  {
    q.x = 0.5 * sqrt(1 + m00 - m11 - m22);
    q.w = (m.cols[1].z - m.cols[2].y) / 4 / q.x;
    q.y = (m.cols[1].x + m.cols[0].y) / 4 / q.x;
    q.z = (m.cols[2].x + m.cols[0].z) / 4 / q.x;
  }

  else if (m11 > m00 && m11 > m22)
  {
    q.y = 0.5 * sqrt(1 + m11 - m00 - m22);
    q.w = (m.cols[2].x - m.cols[0].z) / 4 / q.y;
    q.x = (m.cols[1].x + m.cols[0].y) / 4 / q.y;
    q.z = (m.cols[1].z + m.cols[2].y) / 4 / q.y;
  }

  else if (m22 > m00 && m22 > m11)
  {
    q.z = 0.5 * sqrt(1 + m22 - m11 - m00);
    q.w = (m.cols[0].y - m.cols[1].x) / 4 / q.z;
    q.x = (m.cols[2].x + m.cols[0].z) / 4 / q.z;
    q.y = (m.cols[1].z + m.cols[2].y) / 4 / q.z;
  }

  return dquat_normalize(q);
}

#endif