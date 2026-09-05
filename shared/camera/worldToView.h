#ifndef WORLD_TO_VIEW_H
#define WORLD_TO_VIEW_H

#ifdef __OPENCL_VERSION__
#include "real.cl"
#include "matrix.cl"
#include "quaternion.cl"
typedef real3 dvec3;
#else
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdint>
using dvec3 = glm::dvec3;
using dmat3 = glm::dmat3;
using dquat = glm::dquat;
using glm::mat3_cast;
using glm::quat_cast;
using glm::transpose;
#endif

#ifdef __OPENCL_VERSION__
dvec3 worldToViewSpaceVec(dvec3 position, dvec3 camPosition)
{
  return (dvec3)(position.x, -position.z, position.y) - camPosition;
}
#else
dvec3 worldToViewSpace(const dvec3 &position, const dvec3 &camPosition)
{
  return dvec3(position.x, -position.z, position.y) - camPosition;
}
#endif

#ifdef __OPENCL_VERSION__
dmat3 worldToViewSpaceMat(dmat3 orientation)
{
  dmat3 C;
  C.cols[0] = (dvec3)(1, 0, 0);
  C.cols[1] = (dvec3)(0, 0, 1);
  C.cols[2] = (dvec3)(0, -1, 0);
  return dmat3_dot_dmat3(dmat3_dot_dmat3(C, orientation), dmat3_transpose(C));
}
#else
dmat3 worldToViewSpace(const dmat3 &orientation)
{
  dmat3 C(
      dvec3(1, 0, 0),
      dvec3(0, 0, 1),
      dvec3(0, -1, 0));
  return C * orientation * transpose(C);
}
#endif

#ifdef __OPENCL_VERSION__
dquat worldToViewSpaceQuat(dquat orientation)
{
  real factor = sqrt(2.0) / 2;
  dquat C = (dquat)(factor, 0, 0, factor);
  dquat conjC = (dquat)(-C.x, -C.y, -C.z, C.w);

  return dquat_normalize(dquat_dot_dquat(dquat_dot_dquat(C, orientation), conjC));
}
#else
dquat worldToViewSpace(const dquat &orientation)
{
  dmat3 C(
      dvec3(1, 0, 0),
      dvec3(0, 0, 1),
      dvec3(0, -1, 0));

  dmat3 orientationMat = mat3_cast(orientation);
  dmat3 resultMat = C * orientationMat * transpose(C);
  return quat_cast(resultMat);
}
#endif

#endif