#include "maths/orbitalMaths.h"

#include "physics/constants.h"

#include "physics/structs/keplerElements.h"

double OrbitalMaths::calculateEccentricAnomaly(double M, double e)
{
  double E = M; // initial guess
  double delta;
  do
  {
    delta = (E - e * sin(E) - M) / (1 - e * cos(E));
    E = E - delta;
  } while (abs(delta) > EPS);

  return E;
}

glm::dmat3 OrbitalMaths::createR3matrix(double angle)
{
  return glm::dmat3(
      glm::dvec3(cos(angle), -sin(angle), 0),
      glm::dvec3(sin(angle), cos(angle), 0),
      glm::dvec3(0, 0, 1));
};

glm::dmat3 OrbitalMaths::createR1matrix(double angle)
{
  return glm::dmat3(
      glm::dvec3(1, 0, 0),
      glm::dvec3(0, cos(angle), -sin(angle)),
      glm::dvec3(0, sin(angle), cos(angle)));
}

glm::dvec3 OrbitalMaths::orbitalToInertial(KeplerElements keplerElements, double nu)
{
  if (nu == -1)
  {
    double E = OrbitalMaths::calculateEccentricAnomaly(keplerElements.m, keplerElements.e);
    double e = keplerElements.e;
    nu = atan2(sqrt(1 - e * e) * sin(E) / (1 - e * cos(E)), (cos(E) - e) / (1 - e * cos(E)));
    if (sin(E) < 0)
      nu = 2 * M_PI - nu;
  }

  double r = keplerElements.a * (1 - keplerElements.e * keplerElements.e) / (1 + keplerElements.e * cos(nu));
  glm::dvec3 orb(r * cos(nu), r * sin(nu), 0.0);
  return OrbitalMaths::createR3matrix(keplerElements.Omega) * OrbitalMaths::createR1matrix(keplerElements.i) * OrbitalMaths::createR3matrix(keplerElements.omega) * orb;
}