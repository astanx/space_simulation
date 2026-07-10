#ifndef ORBITAL_MATHS
#define ORBITAL_MATHS

#include "real.cl"
#include "matrix.cl"
#include "constants.cl"

real calculateEccentricAnomaly(real M, real e)
{
  real E = M; // initial guess
  real delta;
  do
  {
    delta = (E - e * sin(E) - M) / (1 - e * cos(E));
    E = E - delta;
  } while (fabs(delta) > EPS);

  return E;
}

dmat3 createR3matrix(real angle)
{
  dmat3 mat;
  mat.cols[0] = (real3)(cos(angle), -sin(angle), 0);
  mat.cols[1] = (real3)(sin(angle), cos(angle), 0);
  mat.cols[2] = (real3)(0, 0, 1);

  return mat;
};

dmat3 createR1matrix(real angle)
{
  dmat3 mat;
  mat.cols[0] = (real3)(1, 0, 0);
  mat.cols[1] = (real3)(0, cos(angle), -sin(angle));
  mat.cols[2] = (real3)(0, sin(angle), cos(angle));

  return mat;
}

real3 orbitalToInertial(real m, real e, real a, real Omega, real i, real omega, real nu)
{
  if (nu == -1)
  {
    real E = calculateEccentricAnomaly(m, e);
    nu = atan2(sqrt(1 - e * e) * sin(E) / (1 - e * cos(E)), (cos(E) - e) / (1 - e * cos(E)));
    if (sin(E) < 0)
      nu = 2 * M_PI - nu;
  }

  real r = a * (1 - e * e) / (1 + e * cos(nu));
  real3 orb = (real3)(r * cos(nu), r * sin(nu), 0.0);
  dmat3 mat = dmat3_dot_dmat3(createR3matrix(Omega), dmat3_dot_dmat3(createR1matrix(i), createR3matrix(omega)));
  return dmat3_dot_d3(mat, orb);
}

#endif