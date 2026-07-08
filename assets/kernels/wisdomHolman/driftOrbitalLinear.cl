#include "kepler.cl"
#include "orbitalMaths.cl"
#include "matrix.cl"

__kernel void driftOrbitalLinear(__global double3* positions, __global double3* velocities, __global double* mu, __global double* semiAxises, 
                          __global double* eccentricities, __global double* inclinations, __global double* longitude, 
                          __global double* periapsis, __global double* meanAnomaly, __global double* meanMotion, 
                          int centralBodyID, double dt)
{
  int id = get_global_id(0);

  double m = meanAnomaly[id];
  double n = meanMotion[id];
  double a = semiAxises[id];
  double e = eccentricities[id];
  double i = inclination[id];
  double Omega = longitude[id];
  double omega = periapsis[id];

  double mu_central = mu[centralBodyID];

  m = advanceMeanAnomaly(m, n, dt);

  double E = calculateEccentricAnomaly(m, e);

  double3 pos = double3(0.0);

  pos.x = a * (cos(E) - e);
  pos.y = a * sqrt(1 - (e * e)) * sin(E);

  double3 v = double3(0.0);
  double r = a * (1 - e * cos(E));

  v.x = -sqrt(mu_central * a) / r * sin(E);
  v.y = sqrt(mu_central * a * (1 - (e * e))) / r * cos(E);

  dmat3 R = createR3matrix(Omega) * createR1matrix(i) * createR3matrix(omega);

  velocities[id] = dot(R, v) + velocities[centralBodyID];
  positions[id] = dot(R, pos) + positions[centralBodyID];

  meanAnomaly[id] = m;
}