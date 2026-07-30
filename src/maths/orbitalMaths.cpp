#include "maths/orbitalMaths.h"
#include "maths/orbitalMathsFormulas.h"

#include "maths/constants.h"

#include "physics/structs/keplerElements.h"

double OrbitalMaths::calculateEccentricAnomaly(double M, double e)
{
  return ::calculateEccentricAnomaly(M, e);
}

glm::dmat3 OrbitalMaths::createR3matrix(double angle)
{
  return ::createR3matrix(angle);
};

glm::dmat3 OrbitalMaths::createR1matrix(double angle)
{
  return ::createR1matrix(angle);
}

glm::dvec3 OrbitalMaths::orbitalToInertial(KeplerElements keplerElements, double nu)
{
  return ::orbitalToInertial(keplerElements.m, keplerElements.e, keplerElements.a, keplerElements.Omega, keplerElements.i, keplerElements.omega, nu);
}