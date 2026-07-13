#include "kepler.cl"
#include "orbitalMaths.cl"
#include "matrix.cl"
#include "real.cl"

__kernel void driftOrbitalLinear(__global real3* positions, __global real* mus, __global real3* velocities, __global real* semiAxises, 
                                __global real* eccentricities, __global real* inclinations, __global real* longitude, 
                                __global real* periapsis, __global real* meanAnomaly, __global real* meanMotion, 
                                __global int* centralBodyIndices, real dt)
{
  int id = get_global_id(0);

  real m = meanAnomaly[id];
  real n = meanMotion[id];
  real a = semiAxises[id];
  real e = eccentricities[id];
  real i = inclinations[id];
  real Omega = longitude[id];
  real omega = periapsis[id];

  int centralBodyID = centralBodyIndices[id];

  real mu_central = mus[centralBodyID];

  m = advanceMeanAnomaly(m, n, dt);

  real E = calculateEccentricAnomaly(m, e);

  real3 pos = real3(0.0);

  pos.x = a * (cos(E) - e);
  pos.y = a * sqrt(1 - (e * e)) * sin(E);

  real3 v = real3(0.0);
  real r = a * (1 - e * cos(E));

  v.x = -sqrt(mu_central * a) / r * sin(E);
  v.y = sqrt(mu_central * a * (1 - (e * e))) / r * cos(E);

  dmat3 R = dmat3_dot_dmat3(createR3matrix(Omega), dmat3_dot_dmat3(createR1matrix(i), createR3matrix(omega)));

  velocities[id] = dmat3_dot_d3(R, v) + velocities[centralBodyID];
  positions[id] = dmat3_dot_d3(R, pos) + positions[centralBodyID];

  meanAnomaly[id] = m;
}