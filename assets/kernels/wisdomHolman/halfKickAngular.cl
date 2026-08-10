#include "matrix.cl"
#include "momentsMaths.cl"
#include "real.cl"

__kernel void halfKickAngular(__global real3* positions, __global real* mus, __global real3* velocities,
                              __global real3* angularVelocities, __global dmat3* quadrupoleTensors, __global dmat3* inertiaTensors, __global real* meanRadii, 
                              __global int* loveIndices, __global int* tidalFactorIndices, 
                              __global real* loveNumbers, __global real* tidalFactors,
                              int count, real dt)
{
  int id = get_global_id(0);
  if (id >= count) return;

  int loveNumberIndex = loveIndices[id]; 
  int tidalFactorIndex = tidalFactorIndices[id];

  ObjectState object;
  object.position = positions[id];
  object.tensor = quadrupoleTensors[id];
  object.angularVelocity = angularVelocities[id];
  object.velocity = velocities[id];
  object.meanRadius = meanRadii[id];

  TidalProperties properties;
  properties.isTidal = loveNumberIndex != -1 && tidalFactorIndex != -1;
  if (properties.isTidal)
  {
    properties.loveNumber = loveNumbers[loveNumberIndex];
    properties.tidalFactor = tidalFactors[tidalFactorIndex];
  }

  real3 torque = calculateTorque(object, properties, positions, velocities, mus, id, count);

  real3 omega = angularVelocities[id];

  dmat3 inertiaTensor = inertiaTensors[id];
  real3 acc = dmat3_dot_d3(dmat3_inverse(inertiaTensor), torque - cross(omega, dmat3_dot_d3(inertiaTensor, omega)));

  angularVelocities[id] += acc * dt;
}
