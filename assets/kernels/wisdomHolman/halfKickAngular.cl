#include "matrix.cl"
#include "momentsMaths.cl"
#include "real.cl"

__kernel void halfKickAngular(__global real3* positions, __global real* mus, __global real3* velocities,
                              __global real3* angularVelocities, __global dmat3* quadrupoleTensors, __global dmat3* inertiaTensors, __global dquat* orientations, __global real* meanRadii, 
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

  dmat3 inertiaTensor = inertiaTensors[id];

  dquat q = orientations[id];

  dmat3 R = dquat_to_dmat3(q);
  dmat3 transR = dmat3_transpose(R);     
  real3 omega = dmat3_dot_d3(transR, angularVelocities[id]);
  real3 torque = dmat3_dot_d3(transR, calculateTorque(object, properties, positions, velocities, mus, id, count));

  real3 acc = dmat3_dot_d3(dmat3_inverse(inertiaTensor), torque - cross(omega, dmat3_dot_d3(inertiaTensor, omega)));

  angularVelocities[id] = dmat3_dot_d3(R, omega + acc * dt);
}
