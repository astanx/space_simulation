#ifndef MATHS_CONSTANTS_H
#define MATHS_CONSTANTS_H

#ifdef __OPENCL_VERSION__
#include "real.cl"
#else
using real = double;
const double AU_TO_METER = 1.49597870700e11;
const double JD_2000 = 2451545.0;
#endif

#define PI ((real)3.14159265358979323846)
#define EPS ((real)1e-12)
#define G ((real)6.674e-11)

#endif