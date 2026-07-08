#ifndef MATRIX
#define MATRIX

typedef struct {
  double4 cols[4];
} dmat4;

typedef struct {
  double3 cols[3];
} dmat3;

typedef struct {
  float4 cols[4];
} mat4;

typedef struct {
  float3 cols[3];
} mat3;

double4 dot(dmat4 m, double4 v)
{
  return  m.cols[0] * v.x +
          m.cols[1] * v.y +
          m.cols[2] * v.z +
          m.cols[3] * v.w;
}

double3 dot(dmat3 m, double3 v)
{
  return  m.cols[0] * v.x +
          m.cols[1] * v.y +
          m.cols[2] * v.z;
}

float4 dot(mat4 m, float4 v)
{
  return  m.cols[0] * v.x +
          m.cols[1] * v.y +
          m.cols[2] * v.z +
          m.cols[3] * v.w;
}

float3 dot(mat3 m, float3 v)
{
  return  m.cols[0] * v.x +
          m.cols[1] * v.y +
          m.cols[2] * v.z;
}

dmat3 identity()
{
  dmat3 mat;
  mat.cols[0] = double3(1, 0, 0);
  mat.cols[1] = double3(0, 1, 0);
  mat.cols[2] = double3(0, 0, 1);

  return mat;
}

#endif