#ifndef ORBITAL_MATHS_FORMULAS_H
#define ORBITAL_MATHS_FORMULAS_H

#ifdef __OPENCL_VERSION__
#include "real.cl"
#include "matrix.cl"
#else
#include <glm/glm.hpp>
#include <cmath>
#endif

#include "maths/constants.h"

#ifdef __OPENCL_VERSION__
inline real calculateEccentricAnomaly(real M, real e)
#else
template <typename real>
inline real calculateEccentricAnomaly(real M, real e)
#endif
{
  real E = M; // initial guess
  real delta;
  int tries = 0;
  do
  {
    delta = (E - e * sin(E) - M) / (1 - e * cos(E));
    E = E - delta;
    tries++;
  } while (fabs(delta) > EPS && tries < 100);

  return E;
}
#ifdef __OPENCL_VERSION__
inline dmat3 createR3matrix(real angle)
#else
template <typename real>
inline glm::mat<3, 3, real> createR3matrix(real angle)
#endif
{
#ifndef __OPENCL_VERSION__
  using dmat3 = glm::mat<3, 3, real>;
  using real3 = glm::vec<3, real>;
#endif

  dmat3 mat;
#ifdef __OPENCL_VERSION__
  mat.cols[0] = (real3)(cos(angle), -sin(angle), 0);
  mat.cols[1] = (real3)(sin(angle), cos(angle), 0);
  mat.cols[2] = (real3)(0, 0, 1);
#else
  mat[0] = real3(cos(angle), -sin(angle), 0);
  mat[1] = real3(sin(angle), cos(angle), 0);
  mat[2] = real3(0, 0, 1);
#endif

  return mat;
};

#ifdef __OPENCL_VERSION__
inline dmat3 createR1matrix(real angle)
#else
template <typename real>
inline glm::mat<3, 3, real> createR1matrix(real angle)
#endif
{
#ifndef __OPENCL_VERSION__
  using dmat3 = glm::mat<3, 3, real>;
  using real3 = glm::vec<3, real>;
#endif
  dmat3 mat;
#ifdef __OPENCL_VERSION__
  mat.cols[0] = (real3)(1, 0, 0);
  mat.cols[1] = (real3)(0, cos(angle), -sin(angle));
  mat.cols[2] = (real3)(0, sin(angle), cos(angle));
#else
  mat[0] = real3(1, 0, 0);
  mat[1] = real3(0, cos(angle), -sin(angle));
  mat[2] = real3(0, sin(angle), cos(angle));
#endif
  return mat;
}

#ifdef __OPENCL_VERSION__
inline real3 orbitalToInertial(real m, real e, real a, real Omega, real i, real omega, real nu)
#else
template <typename real>
inline glm::vec<3, real> orbitalToInertial(real m, real e, real a, real Omega, real i, real omega, real nu)
#endif
{
#ifndef __OPENCL_VERSION__
  using dmat3 = glm::mat<3, 3, real>;
  using real3 = glm::vec<3, real>;
#endif

  if (nu == -1)
  {
    real E = calculateEccentricAnomaly(m, e);
    nu = atan2(sqrt(1 - e * e) * sin(E) / (1 - e * cos(E)), (cos(E) - e) / (1 - e * cos(E)));
    if (sin(E) < 0)
      nu = 2 * M_PI - nu;
  }

  real r = a * (1 - e * e) / (1 + e * cos(nu));
#ifdef __OPENCL_VERSION__
  real3 orb = (real3)(r * cos(nu), r * sin(nu), 0.0);
  dmat3 mat = dmat3_dot_dmat3(createR3matrix(Omega), dmat3_dot_dmat3(createR1matrix(i), createR3matrix(omega)));
  return dmat3_dot_d3(mat, orb);
#else
  real3 orb = real3(r * cos(nu), r * sin(nu), 0.0);
  dmat3 mat = createR3matrix(Omega) * createR1matrix(i) * createR3matrix(omega);
  return mat * orb;
#endif
}

#endif