#ifndef MATRIX
#define MATRIX

#include "real.cl"
#include "maths/constants.h"

typedef struct {
  real4 cols[4];
} dmat4;

typedef struct {
  real3 cols[3];
} dmat3;

typedef struct {
  float4 cols[4];
} mat4;

typedef struct {
  float3 cols[3];
} mat3;

real4 dmat4_dot_d4(dmat4 m, real4 v)
{
  return  m.cols[0] * v.x +
          m.cols[1] * v.y +
          m.cols[2] * v.z +
          m.cols[3] * v.w;
}

real3 dmat3_dot_d3(dmat3 m, real3 v)
{
  return  m.cols[0] * v.x +
          m.cols[1] * v.y +
          m.cols[2] * v.z;
}

float4 mat4_dot_f4(mat4 m, float4 v)
{
  return  m.cols[0] * v.x +
          m.cols[1] * v.y +
          m.cols[2] * v.z +
          m.cols[3] * v.w;
}

float3 mat3_dot_f3(mat3 m, float3 v)
{
  return  m.cols[0] * v.x +
          m.cols[1] * v.y +
          m.cols[2] * v.z;
}

dmat4 dmat4_dot_dmat4(dmat4 m1, dmat4 m2)
{
  dmat4 mat;
  mat.cols[0] = dmat4_dot_d4(m1, m2.cols[0]);
  mat.cols[1] = dmat4_dot_d4(m1, m2.cols[1]);
  mat.cols[2] = dmat4_dot_d4(m1, m2.cols[2]);
  mat.cols[3] = dmat4_dot_d4(m1, m2.cols[3]);

  return mat;
}

dmat3 dmat3_dot_dmat3(dmat3 m1, dmat3 m2)
{
  dmat3 mat;
  mat.cols[0] = dmat3_dot_d3(m1, m2.cols[0]);
  mat.cols[1] = dmat3_dot_d3(m1, m2.cols[1]);
  mat.cols[2] = dmat3_dot_d3(m1, m2.cols[2]);

  return mat;
}

mat4 mat4_dot_mat4(mat4 m1, mat4 m2)
{
  mat4 mat;
  mat.cols[0] = mat4_dot_f4(m1, m2.cols[0]);
  mat.cols[1] = mat4_dot_f4(m1, m2.cols[1]);
  mat.cols[2] = mat4_dot_f4(m1, m2.cols[2]);
  mat.cols[3] = mat4_dot_f4(m1, m2.cols[3]);

  return mat;
}

mat3 mat3_dot_mat3(mat3 m1, mat3 m2)
{
  mat3 mat;
  mat.cols[0] = mat3_dot_f3(m1, m2.cols[0]);
  mat.cols[1] = mat3_dot_f3(m1, m2.cols[1]);
  mat.cols[2] = mat3_dot_f3(m1, m2.cols[2]);

  return mat;
}

dmat4 dmat4_dot_d(dmat4 m, real n)
{
  dmat4 mat;
  mat.cols[0] = m.cols[0] * n;
  mat.cols[1] = m.cols[1] * n;
  mat.cols[2] = m.cols[2] * n;
  mat.cols[3] = m.cols[3] * n;

  return mat;
}

dmat3 dmat3_dot_d(dmat3 m, real n)
{
  dmat3 mat;
  mat.cols[0] = m.cols[0] * n;
  mat.cols[1] = m.cols[1] * n;
  mat.cols[2] = m.cols[2] * n;

  return mat;
}

mat4 mat4_dot_f(mat4 m, float n)
{
  mat4 mat;
  mat.cols[0] = m.cols[0] * n;
  mat.cols[1] = m.cols[1] * n;
  mat.cols[2] = m.cols[2] * n;
  mat.cols[3] = m.cols[3] * n;

  return mat;
}

mat3 mat3_dot_f(mat3 m, float n)
{
  mat3 mat;
  mat.cols[0] = m.cols[0] * n;
  mat.cols[1] = m.cols[1] * n;
  mat.cols[2] = m.cols[2] * n;

  return mat;
}

dmat4 dmat4_plus_dmat4(dmat4 m1, dmat4 m2)
{
  dmat4 mat;
  mat.cols[0] = m1.cols[0] + m2.cols[0];
  mat.cols[1] = m1.cols[1] + m2.cols[1];
  mat.cols[2] = m1.cols[2] + m2.cols[2];
  mat.cols[3] = m1.cols[3] + m2.cols[3];

  return mat;
}

dmat3 dmat3_plus_dmat3(dmat3 m1, dmat3 m2)
{
  dmat3 mat;
  mat.cols[0] = m1.cols[0] + m2.cols[0];
  mat.cols[1] = m1.cols[1] + m2.cols[1];
  mat.cols[2] = m1.cols[2] + m2.cols[2];

  return mat;
}

mat4 mat4_plus_mat4(mat4 m1, mat4 m2)
{
  mat4 mat;
  mat.cols[0] = m1.cols[0] + m2.cols[0];
  mat.cols[1] = m1.cols[1] + m2.cols[1];
  mat.cols[2] = m1.cols[2] + m2.cols[2];
  mat.cols[3] = m1.cols[3] + m2.cols[3];

  return mat;
}

mat3 mat3_plus_mat3(mat3 m1, mat3 m2)
{
  mat3 mat;
  mat.cols[0] = m1.cols[0] + m2.cols[0];
  mat.cols[1] = m1.cols[1] + m2.cols[1];
  mat.cols[2] = m1.cols[2] + m2.cols[2];

  return mat;
}

dmat3 dmat3_identity()
{
  dmat3 mat;
  mat.cols[0] = (real3)(1, 0, 0);
  mat.cols[1] = (real3)(0, 1, 0);
  mat.cols[2] = (real3)(0, 0, 1);

  return mat;
}

dmat4 dmat4_identity()
{
  dmat4 mat;
  mat.cols[0] = (real4)(1, 0, 0, 0);
  mat.cols[1] = (real4)(0, 1, 0, 0);
  mat.cols[2] = (real4)(0, 0, 1, 0);
  mat.cols[3] = (real4)(0, 0, 0, 1);

  return mat;
}

mat3 mat3_identity()
{
  mat3 mat;
  mat.cols[0] = (float3)(1, 0, 0);
  mat.cols[1] = (float3)(0, 1, 0);
  mat.cols[2] = (float3)(0, 0, 1);

  return mat;
}

mat4 mat4_identity()
{
  mat4 mat;
  mat.cols[0] = (float4)(1, 0, 0, 0);
  mat.cols[1] = (float4)(0, 1, 0, 0);
  mat.cols[2] = (float4)(0, 0, 1, 0);
  mat.cols[3] = (float4)(0, 0, 0, 1);

  return mat;
}

dmat4 dmat4_inverse(dmat4 matrix)
{
  real a = matrix.cols[0].x;
  real e = matrix.cols[0].y;
  real i = matrix.cols[0].z;
  real m = matrix.cols[0].w;

  real b = matrix.cols[1].x;
  real f = matrix.cols[1].y;
  real j = matrix.cols[1].z;
  real n = matrix.cols[1].w;

  real c = matrix.cols[2].x;
  real g = matrix.cols[2].y;
  real k = matrix.cols[2].z;
  real o = matrix.cols[2].w;

  real d = matrix.cols[3].x;
  real h = matrix.cols[3].y;
  real l = matrix.cols[3].z;
  real p = matrix.cols[3].w;

  real A11 = f * (k * p - l * o) - g * (j * p - l * n) + h * (j * o - k * n);
  real A12 = -(e * (k * p - l * o) - g * (i * p - l * m) + h * (i * o - l * m));
  real A13 = e * (j * p - l * o) - f * (i * p - l * m) + h * (i * n - j * m);
  real A14 = -(e * (j * o - k * n) - f * (i * o - k * m) + g * (i * n - j * m));

  real det = a * A11 + b * A12 + c * A13 + d * A14;

  if (fabs(det) < EPS)
    return dmat4_identity();

  real invDet = 1.0 / det;

  dmat4 mat;
  mat.cols[0] = (real4)(A11, A12, A13, A14) * invDet;

  mat.cols[1] = (real4)(
      -(b * (k * p - l * o) - c * (j * p - l * n) + d * (j * o - k * n)),
      a * (k * p - l * o) - c * (i * p - l * m) + d * (i * o - k * m),
      -(a * (j * p - l * o) - b * (i * p - l * m) + d * (i * n - j * m)),
      a * (j * o - k * n) - b * (i * o - k * m) + c * (i * n - j * m)
  ) * invDet;

  mat.cols[2] = (real4)(
      b * (g * p - h * o) - c * (f * p - h * n) + d * (f * o - g * n),
      -(a * (g * p - h * o) - c * (e * p - h * m) + d * (e * o - g * m)),
      a * (f * p - h * n) - b * (e * p - h * m) + d * (e * n - f * m),
      -(a * (f * o - g * n) - b * (e * o - g * m) + c * (e * n - f * m))
  ) * invDet;

  mat.cols[3] = (real4)(
     -(b * (g * l - h * k) - c * (f * l - h * j) + d * (f * k - g * j)),
     a * (g * l - h * k) - c * (e * l - h * i) + d * (e * k - g * i),
     -(a * (f * l - h * j) - b * (e * l - h * i) + d * (e * j - f * i)),
     a * (f * k - g * j) - b * (e * k - g * i) + c * (e * j - f * i)
  ) * invDet;

  return mat;
}

dmat3 dmat3_inverse(dmat3 m)
{
  real a = m.cols[0].x;
  real d = m.cols[0].y;
  real g = m.cols[0].z;

  real b = m.cols[1].x;
  real e = m.cols[1].y;
  real h = m.cols[1].z;

  real c = m.cols[2].x;
  real f = m.cols[2].y;
  real i = m.cols[2].z;

  real det = a * (e * i - f * h) -
                b * (d * i - f * g) +
                c * (d * h - e * g);

  if (fabs(det) < EPS)
    return dmat3_identity();

  real invDet = 1.0 / det;

  dmat3 mat;
  mat.cols[0] = (real3)(
      (e * i - f * h),
      (f * g - d * i),
      (d * h - e * g)
  ) * invDet;

  mat.cols[1] = (real3)(
      (c * h - b * i),
      (a * i - c * g),
      (b * g - a * h)
  ) * invDet;

  mat.cols[2] = (real3)(
      (b * f - c * e),
      (c * d - a * f),
      (a * e - b * d)
  ) * invDet;

  return mat;
}

mat4 mat4_inverse(mat4 matrix)
{
  float a = matrix.cols[0].x;
  float e = matrix.cols[0].y;
  float i = matrix.cols[0].z;
  float m = matrix.cols[0].w;

  float b = matrix.cols[1].x;
  float f = matrix.cols[1].y;
  float j = matrix.cols[1].z;
  float n = matrix.cols[1].w;

  float c = matrix.cols[2].x;
  float g = matrix.cols[2].y;
  float k = matrix.cols[2].z;
  float o = matrix.cols[2].w;

  float d = matrix.cols[3].x;
  float h = matrix.cols[3].y;
  float l = matrix.cols[3].z;
  float p = matrix.cols[3].w;

  float A11 = f * (k * p - l * o) - g * (j * p - l * n) + h * (j * o - k * n);
  float A12 = -(e * (k * p - l * o) - g * (i * p - l * m) + h * (i * o - l * m));
  float A13 = e * (j * p - l * o) - f * (i * p - l * m) + h * (i * n - j * m);
  float A14 = -(e * (j * o - k * n) - f * (i * o - k * m) + g * (i * n - j * m));

  float det = a * A11 + b * A12 + c * A13 + d * A14;

  if (fabs(det) < EPS)
    return mat4_identity();

  float invDet = 1.0 / det;

  mat4 mat;
  mat.cols[0] = (float4)(A11, A12, A13, A14) * invDet;

  mat.cols[1] = (float4)(
      -(b * (k * p - l * o) - c * (j * p - l * n) + d * (j * o - k * n)),
      a * (k * p - l * o) - c * (i * p - l * m) + d * (i * o - k * m),
      -(a * (j * p - l * o) - b * (i * p - l * m) + d * (i * n -j * m)),
      a * (j * o - k * n) - b * (i * o - k * m) + c * (i * n - j * m)
  ) * invDet;

  mat.cols[2] = (float4)(
      b * (g * p - h * o) - c * (f * p - h * n) + d * (f * o - g * n),
      -(a * (g * p - h * o) - c * (e * p - h * m) + d * (e * o - g * m)),
      a * (f * p - h * n) - b * (e * p - h * m) + d * (e * n - f * m),
      -(a * (f * o - g * n) - b * (e * o - g * m) + c * (e * n - f * m))
  ) * invDet;

  mat.cols[3] = (float4)(
     -(b * (g * l - h * k) - c * (f * l - h * j) + d * (f * k - g * j)),
     a * (g * l - h * k) - c * (e * l - h * i) + d * (e * k - g * i),
     -(a * (f * l - h * j) - b * (e * l - h * i) + d * (e * j - f * i)),
     a * (f * k - g * j) - b * (e * k - g * i) + c * (e * j - f * i)
  ) * invDet;

  return mat;
}

mat3 mat3_inverse(mat3 m)
{
  float a = m.cols[0].x;
  float d = m.cols[0].y;
  float g = m.cols[0].z;

  float b = m.cols[1].x;
  float e = m.cols[1].y;
  float h = m.cols[1].z;

  float c = m.cols[2].x;
  float f = m.cols[2].y;
  float i = m.cols[2].z;

  float det = a * (e * i - f * h) -
                b * (d * i - f * g) +
                c * (d * h - e * g);

  if (fabs(det) < EPS)
    return mat3_identity();

  float invDet = 1.0 / det;

  mat3 mat;
  mat.cols[0] = (float3)(
      (e * i - f * h),
      (f * g - d * i),
      (d * h - e * g)
  ) * invDet;

  mat.cols[1] = (float3)(
      (c * h - b * i),
      (a * i - c * g),
      (b * g - a * h)
  ) * invDet;

  mat.cols[2] = (float3)(
      (b * f - c * e),
      (c * d - a * f),
      (a * e - b * d)
  ) * invDet;

  return mat;
}

mat3 mat3_transpose(mat3 m)
{
  mat3 m2;
  m2.cols[0] = (float3)(m.cols[0].x, m.cols[1].x, m.cols[2].x);
  m2.cols[1] = (float3)(m.cols[0].y, m.cols[1].y, m.cols[2].y);
  m2.cols[2] = (float3)(m.cols[0].z, m.cols[1].z, m.cols[2].z);

  return m2;
}

dmat3 dmat3_transpose(dmat3 m)
{
  dmat3 m2;
  m2.cols[0] = (real3)(m.cols[0].x, m.cols[1].x, m.cols[2].x);
  m2.cols[1] = (real3)(m.cols[0].y, m.cols[1].y, m.cols[2].y);
  m2.cols[2] = (real3)(m.cols[0].z, m.cols[1].z, m.cols[2].z);

  return m2;
}


#endif