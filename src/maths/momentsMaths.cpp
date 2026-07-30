#include "maths/momentsMaths.h"

#include "maths/constants.h"

#include "maths/torqueMaths.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/constants.h"

#include "physics/structs/radii.h"
#include "physics/structs/gravityField.h"
#include "physics/structs/inertiaProperties.h"
#include "physics/structs/tidalParameters.h"

#include <glm/gtc/matrix_transform.hpp>

double MomentsMaths::calculateA(double mass, Radii radii)
{
  return mass / 5 * (radii.equatorian * radii.equatorian + radii.polar * radii.polar);
}

double MomentsMaths::calculateA(double mass, Radii radii, GravityField gravityField)
{
  if (gravityField.C == -1 && gravityField.C20 == -1 && gravityField.C22 == -1)
    return calculateA(mass, radii);
  return mass * radii.mean * radii.mean * (gravityField.C + gravityField.C20 * sqrt(5) - 2 * gravityField.C22 * sqrt(15));
}

double MomentsMaths::calculateB(double mass, Radii radii)
{
  return mass / 5 * (radii.equatorian * radii.equatorian + radii.polar * radii.polar);
}

double MomentsMaths::calculateB(double mass, Radii radii, GravityField gravityField)
{
  if (gravityField.C == -1 && gravityField.C20 == -1 && gravityField.C22 == -1)
    return calculateB(mass, radii);
  return mass * radii.mean * radii.mean * (gravityField.C + gravityField.C20 * sqrt(5) + 2 * gravityField.C22 * sqrt(15));
}

double MomentsMaths::calculateC(double mass, Radii radii)
{
  return mass / 5 * (2 * radii.equatorian * radii.equatorian);
}

double MomentsMaths::calculateC(double mass, Radii radii, GravityField gravityField)
{
  if (gravityField.C == -1 && gravityField.C20 == -1 && gravityField.C22 == -1)
    return calculateC(mass, radii);
  return mass * radii.mean * radii.mean * gravityField.C;
}

double MomentsMaths::calculateC20(double mass, Radii radii, InertiaProperties properties)
{
  return -(properties.C - (properties.A + properties.B) / 2) / (mass * radii.mean * radii.mean * sqrt(5));
}

double MomentsMaths::calculateC22(double mass, Radii radii, InertiaProperties properties)
{
  return (properties.B - properties.A) / (4 * mass * radii.mean * radii.mean);
}

glm::dmat3 MomentsMaths::calculateQuadrupoleTensor(double mass, Radii radii, InertiaProperties properties, GravityField field)
{
  double C_20 = field.C20 == -1 ? MomentsMaths::calculateC20(mass, radii, properties) : field.C20;
  double C_22 = field.C22 == -1 ? MomentsMaths::calculateC22(mass, radii, properties) : field.C22;
  return glm::dmat3(glm::dvec3(-C_20 + 2 * C_22, 0, 0), glm::dvec3(0, -C_20 - 2 * C_22, 0), glm::dvec3(0, 0, 2 * C_20)) *
         mass * radii.mean * radii.mean;
}

glm::dvec3 MomentsMaths::calculateTorque(Object *object, const std::vector<Object *> &bodies)
{
  glm::dvec3 torque = glm::dvec3(0);
  for (const Object *body : bodies)
  {
    if (body == object)
      continue;

    torque += MomentsMaths::calculateGravitationalTorqueForObject(object, body);
    torque += MomentsMaths::calculateTidalTorqueForObject(object, body);
  }
  return torque;
}

glm::dvec3 MomentsMaths::calculateGravitationalTorqueForObject(Object *object, const Object *body)
{
  glm::dvec3 dp = body->getPosition() - object->getPosition();
  double d = glm::length(dp);
  return calculateGravitationalTorque(dp, d, object->getQuadrupoleTensor(), body->getMu());
}

glm::dvec3 MomentsMaths::calculateTidalTorqueForObject(Object *object, const Object *body)
{
  const TidalParameters &p = object->getTidalParameters();
  if (p.k2 == -1 && p.Q == -1)
    return glm::dvec3(0.0);

  double mu = body->getMu();
  glm::dvec3 dp = object->getPosition() - body->getPosition();
  double d = glm::length(dp);

  return calculateTidalTorque(dp, d, object->getAngularVelocity(), object->getVelocity(), object->getRadius(), p.k2, p.Q, body->getVelocity(), body->getMu());
}