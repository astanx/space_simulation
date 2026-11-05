#include "real.cl"

#include "physics/structs/keplerElements.h"
#include "physics/structs/keplerMaths.h"

__kernel void updateKeplerElements(__global real3* positions, __global real* mus, __global real3* velocities, __global real* semiAxises, 
                                __global real* eccentricities, __global real* inclinations, __global real* longitude, 
                                __global real* periapsis, __global real* meanAnomaly, __global real* meanMotion, 
                                __global int* centralBodyIndices, real dt)
{
  int id = get_global_id(0);

  int centralBodyID = centralBodyIndices[id];

  real3 pos = positions[id] - positions[centralBodyID];
  real3 vel = velocities[id] - velocities[centralBodyID];
  real mu = mus[id] + mus[centralBodyID];

  KeplerElements elements = calculateKeplerElements(mu, pos, vel);

  semiAxises[id] = elements.a;
  eccentricities[id] = elements.e;
  inclinations[id] = elements.i;
  longitude[id] = elements.Omega;
  periapsis[id] = elements.omega;
  meanAnomaly[id] = elements.m;
  meanMotion[id] = elements.n;
}