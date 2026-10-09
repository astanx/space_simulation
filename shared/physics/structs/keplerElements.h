#ifndef KEPLER_ELEMENTS_H
#define KEPLER_ELEMENTS_H

#ifdef __OPENCL_VERSION__
#include "real.cl"
#else
#include <cmath>
#include <glm/glm.hpp>
using std::fmod;
using std::sqrt;
#endif

#ifndef __OPENCL_VERSION__
template <typename real>
struct KeplerElements
#else
typedef struct
#endif
{
  real a; // semi-major axis (meters)
  real e; // eccentricity (unitless)

  real i;     // inclination (radians)
  real Omega; // longitude of ascending node (radians)
  real omega; // argument of periapsis (radians)

  real m; // mean anomaly (radians)

  real n; // mean motion (radians per second)

#ifndef __OPENCL_VERSION__
  KeplerElements() = default;
  template <typename other>
  KeplerElements(KeplerElements<other> e) : a(e.a), e(e.e), i(e.i), Omega(e.Omega), omega(e.omega), m(e.m), n(e.n) {}
  KeplerElements(real a, real e, real i, real Omega, real omega, real m, real n = 0) : a(a), e(e), i(i), Omega(Omega), omega(omega), m(m), n(n) {}
#endif
}
#ifndef __OPENCL_VERSION__
;
#else
KeplerElements;
#endif

#endif