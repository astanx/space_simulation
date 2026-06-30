#include "maths/momentsMaths.h"

#include "maths/constants.h"

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

    torque += MomentsMaths::calculateGravitationalTorque(object, body);
    torque += MomentsMaths::calculateTidalTorque(object, body);
  }
  return torque;
}

glm::dvec3 MomentsMaths::calculateGravitationalTorque(Object *object, const Object *body)
{
  glm::dvec3 R = body->getPosition() - object->getPosition();
  double d = glm::length(R);

  if (d == 0.0)
    return glm::dvec3(0.0);

  glm::dvec3 r = R / d;
  return 3 * G * body->getMass() / std::pow(d, 3) * glm::cross(r, (object->getQuadrupoleTensor() * r));
}

glm::dvec3 MomentsMaths::calculateTidalTorque(Object *object, const Object *body)
{
  const TidalParameters &p = object->getTidalParameters();
  if (p.k2 == -1 && p.Q == -1)
    return glm::dvec3(0.0);

  double mu = body->getMu();
  glm::dvec3 r = object->getPosition() - body->getPosition();
  double d = glm::length(r);
  if (d == 0.0)
    return glm::dvec3(0.0);
  glm::dvec3 omega = object->getAngularVelocity();
  glm::dvec3 v = object->getVelocity() - body->getVelocity();

  glm::dvec3 nVec = glm::cross(r, v) / glm::dot(r, r);
  double n = glm::length(nVec);

  if (n < EPS)
    return glm::dvec3(0.0);

  return -3 * p.k2 * 1 / (2 * n * p.Q) * mu * mu * pow(object->getRadius(), 5) / G / pow(d, 6) * (omega - nVec);
}