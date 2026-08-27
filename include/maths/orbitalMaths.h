#pragma once

#include <glm/glm.hpp>

#include "physics/structs/keplerElements.h"
#include "maths/orbitalMathsFormulas.h"

struct Radii;

namespace OrbitalMaths
{
  template <typename real>
  double calculateEccentricAnomaly(real M, real e)
  {
    return ::calculateEccentricAnomaly(M, e);
  };
  template <typename real>
  glm::dvec3 orbitalToInertial(const KeplerElements<real> &keplerElements, real nu = -1.0)
  {
    return ::orbitalToInertial<real>(keplerElements.m, keplerElements.e, keplerElements.a, keplerElements.Omega, keplerElements.i, keplerElements.omega, nu);
  }
  template <typename real>
  glm::dmat3 createR3matrix(real angle)
  {
    return ::createR3matrix<real>(angle);
  }
  template <typename real>
  glm::dmat3 createR1matrix(real angle)
  {
    return ::createR1matrix<real>(angle);
  }
};