#pragma once

#include "debug/logger.h"

#include <glm/glm.hpp>

template <typename To, typename From>
constexpr To cast(From value)
{
  if constexpr (std::is_floating_point_v<To>)
  {
    constexpr To limit = std::numeric_limits<To>::max() * To(0.1);
    if (value > limit)
    {
      Logger::logWarning("GPU Types", "Clamping the value to the positive limit because of float precision");
      return limit;
    }

    if (value < -limit)
    {
      Logger::logWarning("GPU Types", "Clamping the value to the negative limit because of float precision");
      return -limit;
    }

    if (!std::isfinite(value))
    {
      Logger::logWarning("GPU Types", "Setting the value to limit because it is not finite");
      return value > 0 ? limit : -limit;
    }
  }

  return static_cast<To>(value);
}

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
      : x(cast<Real>(v.x)),
        y(cast<Real>(v.y)),
        z(cast<Real>(v.z)),
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
      : x(cast<Real>(q.x)),
        y(cast<Real>(q.y)),
        z(cast<Real>(q.z)),
        w(cast<Real>(q.w))
  {
  }
};