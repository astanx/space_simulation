#ifndef ORBITAL_MATHS
#define ORBITAL_MATHS

#include "matrix.cl"

double calculateEccentricAnomaly(double M, double e)
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

dmat3 createR3matrix(double angle)
{
  dmat3 mat;
  mat.col[0] = double3(cos(angle), -sin(angle), 0);
  mat.col[1] = double3(sin(angle), cos(angle), 0);
  mat.col[2] = double3(0, 0, 1);

  return mat;
};

glm::dmat3 createR1matrix(double angle)
{
  dmat3 mat;
  mat.col[0] = double3(1, 0, 0);
  mat.col[1] = double3(0, cos(angle), -sin(angle));
  mat.col[2] = double3(0, sin(angle), cos(angle));

  return mat;
}

glm::dvec3 orbitalToInertial(double m, double e, double a, double Omega, double i, double omega, double nu)
{
  if (nu == -1)
  {
    double E = calculateEccentricAnomaly(m, e);
    double e = e;
    nu = atan2(sqrt(1 - e * e) * sin(E) / (1 - e * cos(E)), (cos(E) - e) / (1 - e * cos(E)));
    if (sin(E) < 0)
      nu = 2 * M_PI - nu;
  }

  double r = a * (1 - e * e) / (1 + e * cos(nu));
  double3 orb = double3(r * cos(nu), r * sin(nu), 0.0);
  return createR3matrix(Omega) * createR1matrix(i) * createR3matrix(omega) * orb;
}

#endif