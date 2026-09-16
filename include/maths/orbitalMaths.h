#pragma once

#include <glm/glm.hpp>

#include "physics/structs/keplerElements.h"
#include "maths/orbitalMathsFormulas.h"

struct Radii;

namespace OrbitalMaths
{
  template <typename real>
  real calculateEccentricAnomaly(real M, real e)
  {
    return ::calculateEccentricAnomaly(M, e);
  };
  template <typename real>
  glm::vec<3, real> orbitalToInertial(const KeplerElements<real> &keplerElements, real nu = -1.0)
  {
    return ::orbitalToInertial<real>(keplerElements.m, keplerElements.e, keplerElements.a, keplerElements.Omega, keplerElements.i, keplerElements.omega, nu);
  }
  template <typename real>
  glm::mat<3, 3, real> createR3matrix(real angle)
  {
    return ::createR3matrix<real>(angle);
  }
  template <typename real>
  glm::mat<3, 3, real> createR1matrix(real angle)
  {
    return ::createR1matrix<real>(angle);
  }
};