#include "maths/momentsMaths.h"

#include "physics/object.h"
#include "physics/constants.h"

#include "physics/structs/radii.h"
#include "physics/structs/gravityField.h"

#include <glm/gtc/matrix_transform.hpp>

double MomentsMaths::calculateA(double mass, Radii radii)
{
  return mass / 5 * (radii.equatorian * radii.equatorian + radii.polar * radii.polar);
}

double MomentsMaths::calculateA(double mass, Radii radii, GravityField gravityField)
{
  if (gravityField.C == -1 && gravityField.C20 == -1 && gravityField.C22 == -1)
    return calculateA(mass, radii);
  return mass * radii.mean * radii.mean * (gravityField.C + gravityField.C20 * sqrt(5) - 2 * gravityField.C22);
}

double MomentsMaths::calculateB(double mass, Radii radii)
{
  return mass / 5 * (radii.equatorian * radii.equatorian + radii.polar * radii.polar);
}

double MomentsMaths::calculateB(double mass, Radii radii, GravityField gravityField)
{
  if (gravityField.C == -1 && gravityField.C20 == -1 && gravityField.C22 == -1)
    return calculateB(mass, radii);
  return mass * radii.mean * radii.mean * (gravityField.C + gravityField.C20 * sqrt(5) + 2 * gravityField.C22);
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

double MomentsMaths::calculateC20(double mass, Radii radii)
{
  double A = MomentsMaths::calculateA(mass, radii);
  double B = MomentsMaths::calculateB(mass, radii);
  double C = MomentsMaths::calculateC(mass, radii);
  return -(C - (A + B) / 2) / (mass * radii.mean * radii.mean * sqrt(5));
}

double MomentsMaths::calculateC22(double mass, Radii radii)
{
  double A = MomentsMaths::calculateA(mass, radii);
  double B = MomentsMaths::calculateB(mass, radii);
  return (B - A) / (4 * mass * radii.mean * radii.mean);
}

glm::dmat3 MomentsMaths::calculateQuadrupoleTensor(double mass, Radii radii, double C20, double C22)
{
  double C_20 = C20 == -1 ? MomentsMaths::calculateC20(mass, radii) : C20;
  double C_22 = C22 == -1 ? MomentsMaths::calculateC22(mass, radii) : C22;
  return glm::dmat3(glm::dvec3(-C_20 + 2 * C_22, 0, 0), glm::dvec3(0, -C_20 - 2 * C_22, 0), glm::dvec3(0, 0, 2 * C_20)) *
         mass * radii.mean * radii.mean;
}

glm::dvec3 MomentsMaths::calculateTorque(std::vector<Object> &bodies, glm::dmat3 quadropleTensor, glm::dvec3 position)
{
  glm::dvec3 torque = glm::dvec3(0);
  for (const auto &body : bodies)
  {
    glm::dvec3 R = body.getPosition() - position;
    double d = glm::length(R);

    if (d == 0.0)
      continue;

    glm::dvec3 r = R / d;
    torque += 3 * G * body.getMass() / std::pow(d, 3) * glm::cross(r, (quadropleTensor * r));
  }
  return torque;
}