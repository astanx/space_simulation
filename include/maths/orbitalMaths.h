#pragma once

#include <glm/glm.hpp>

struct KeplerElements;
struct Radii;

namespace OrbitalMaths
{
  double calculateEccentricAnomaly(double M, double e);
  glm::dvec3 orbitalToInertial(KeplerElements keplerElements, double nu = -1.0);
  glm::dmat3 createR3matrix(double angle);
  glm::dmat3 createR1matrix(double angle);
}