#pragma once

#include <glm/glm.hpp>

template <typename Real>
struct Vec3
{
  Real x;
  Real y;
  Real z;
  Real pad;

  Vec3() = default;

  template <typename T>
  Vec3(const glm::vec<3, T> &v)
      : x(static_cast<Real>(v.x)),
        y(static_cast<Real>(v.y)),
        z(static_cast<Real>(v.z)),
        pad(0)
  {
  }
};

template <typename Real>
struct Mat3
{
  Vec3<Real> col1;
  Vec3<Real> col2;
  Vec3<Real> col3;

  Mat3() = default;

  template <typename T>
  Mat3(const glm::mat<3, 3, T> &m)
      : col1(m[0]),
        col2(m[1]),
        col3(m[2])
  {
  }
};

template <typename Real>
struct Quat
{
  Real x;
  Real y;
  Real z;
  Real w;

  Quat() = default;

  template <typename T>
  Quat(const glm::qua<T> &q)
      : x(static_cast<Real>(q.x)),
        y(static_cast<Real>(q.y)),
        z(static_cast<Real>(q.z)),
        w(static_cast<Real>(q.w))
  {
  }
};