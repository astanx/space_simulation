#include "matrix.cl"
#include "momentsMaths.cl"

__kernel void halfKickAngular(__global double3* torques, __global double3* angularVelocities, __global dmat3* tensors, 
  __global double3* positions, __global double3* velocities, __global double* mus, __global double* meanRadii, 
  __global int* loveIndices, __global int* tidalFactorIndices, __global double* loveNumbers, __global double* tidalFactors,
  int count, double dt)
{
  int id = get_global_id(0);
  if (id >= count) return;

  int loveNumberIndex = loveIndices[id]; 
  int tidalFactorIndex = tidalFactorIndices[id];

  struct ObjectState object;
  object.position = positions[id];
  object.tensor = tensors[id];
  object.angularVelocity = angularVelocities[id];
  object.velocity = velocities[id];
  object.meanRadius = meanRadii[id];

  struct TidalProperties properties;
  properties.isTidal = loveNumberIndex != -1 && tidalFactorIndex != -1;
  if (properties.isTidal)
  {
    properties.loveNumber = loveNumbers[loveNumberIndex];
    properties.tidalFactor = loveNumbers[tidalFactorIndex];
  }

  double3 torque = calculateTorque(object, properties, positions, velocities, mus, id, count);
  
  double3 omega = angularVelocities[id];
  dmat3 tensor = tensors[id];

  double3 acc = (torque - cross(omega, dot(tensor, omega))) / tensor;

  angularVelocities[id] += acc * dt;
}
