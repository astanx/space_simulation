#pragma once

#include <glm/glm.hpp>

struct KeplerElements;

namespace OrbitalMaths
{
  double calculateEccentricAnomaly(double M, KeplerElements keplerElements);
  glm::dvec3 orbitalToInertial(KeplerElements keplerElements, double nu = -1.0);
  glm::dmat3 createR3matrix(double angle);
  glm::dmat3 createR1matrix(double angle);
}