#include "matrix.cl"
#include "constants.cl"

struct ObjectState
{
  double3 position;
  dmat3 tensor;
  double3 angularVelocity;
  double3 velocity;
  double meanRadius;
}

struct TidalProperties
{
  double loveNumber;
  double tidalFactor;
  bool isTidal;
}

double3 calculateTorque(ObjectState object, TidalProperties properties, 
                        __global double3* positions, __global double3* velocities, __global double3* mus, 
                        int i, int count)
{
  double3 torque = double3(0.0);

  for (int j = 0; j < count; j++)
  {
    if (i == j)
      continue;

    double3 bodyPosition = positions[j];
    double bodyMu = mus[j];

    torque += calculateGravitationalTorque(object.position, object.tensor, bodyMu, bodyPosition);
    if (properties.isTidal)
      torque += calculateTidalTorque(object.position, object.angularVelocity, object.velocity, 
                                      object.meanRadius, properties.loveNumber, properties.tidalFactor, 
                                      velocities[j], bodyPosition, bodyMu);
  }
  return torque;
}

double3 calculateGravitationalTorque(double3 objectPosition, dmat3 objectTensor, double bodyMu, double3 bodyPosition)
{
  double3 R = bodyPosition - objectPosition;
  double d = length(R);

  if (d == 0.0)
    return double3(0.0);

  double3 r = R / d;
  return 3 *  bodyMu / pow(d, 3) * cross(r, dot(objectTensor, r));
}

double3 calculateTidalTorque(double3 objectPosition, double3 objectAngularVelocity, 
                              double3 objectVelocity, double objectMeanRadius,
                              double objectLoveNumber, double objectTidalFactor,
                              double3 bodyVelocity, double3 bodyPosition, double bodyMu)
{
  double3 r = objectPosition - bodyPosition;
  double d = length(r);
  if (d == 0.0)
    return double3(0.0);

  double3 v = objectVelocity - bodyVelocity;

  double3 nVec = cross(r, v) / dot(r, r);
  double n = length(nVec);

  if (n < EPS)
    return double3(0.0);

  return -3 * objectLoveNumber * 1 / (2 * n * objectTidalFactor) * mu * mu * pow(objectMeanRadius, 5) / G / pow(d, 6) * (objectAngularVelocity - nVec);
}