#include "matrix.cl"
#include "momentsMaths.cl"
#include "real.cl"

__kernel void halfKick(__global real3* positions, __global real* mus, __global real3* velocities,
                              __global real3* angularVelocities, __global dmat3* tensors, __global real* meanRadii, 
                              __global int* centralBodyIndices,
                              __global int* loveIndices, __global int* tidalFactorIndices, 
                              __global real* loveNumbers, __global real* tidalFactors,
                              int count, real dt)
{
  int id = get_global_id(0);
  if (id >= count) return;

  int centralBodyID = centralBodyIndices[id];
  real3 position = positions[id];

  real3 omega = angularVelocities[id];

  int loveNumberIndex = loveIndices[id]; 
  int tidalFactorIndex = tidalFactorIndices[id];

  ObjectState object;
  object.position = position;
  object.tensor = tensors[id];
  object.angularVelocity = omega;
  object.velocity = velocities[id];
  object.meanRadius = meanRadii[id];

  TidalProperties properties;
  properties.isTidal = loveNumberIndex != -1 && tidalFactorIndex != -1;
  if (properties.isTidal)
  {
    properties.loveNumber = loveNumbers[loveNumberIndex];
    properties.tidalFactor = tidalFactors[tidalFactorIndex];
  }

  real3 torque = (real3)(0.0);
  real3 linear_acc = (real3)(0.0);
  for (int j = 0; j < count; j++)
  {
    if (id == j)
      continue;

    real3 bodyPosition = positions[j];
    real bodyMu = mus[j];
    real3 dp = bodyPosition - position;

    real distSq = dot(dp, dp);
    if (distSq < EPS) break; // Avoid singularity
    real dist = sqrt(distSq);
    
    // Linear
    if (id != centralBodyID)
      linear_acc += dp * bodyMu / (dist * distSq);

    // Angular
    torque += calculateGravitationalTorque(dp, dist, object.tensor, bodyMu);
    if (properties.isTidal)
      torque += calculateTidalTorque(dp, dist, object.angularVelocity, object.velocity, 
                                      object.meanRadius, properties.loveNumber, properties.tidalFactor, 
                                      velocities[j], bodyMu);
  }
  
  dmat3 tensor = tensors[id];

  real3 angular_acc = dmat3_dot_d3(dmat3_inverse(tensor), torque - cross(omega, dmat3_dot_d3(tensor, omega)));

  angularVelocities[id] += angular_acc * dt;
  velocities[id] += linear_acc * dt;
}
