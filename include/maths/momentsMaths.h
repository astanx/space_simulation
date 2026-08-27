#pragma once

#include "physics/structs/gravityField.h"
#include "physics/structs/radii.h"
#include "physics/structs/tidalParameters.h"

#include "physics/object.h"

#include <glm/glm.hpp>
#include <vector>

namespace MomentsMaths
{
  template <typename real>
  double calculateA(real mass, Radii radii)
  {
    return mass / 5 * (radii.equatorian * radii.equatorian + radii.polar * radii.polar);
  }
  template <typename real>
  real calculateA(real mass, Radii radii, GravityField gravityField)
  {
    if (gravityField.C == -1 && gravityField.C20 == -1 && gravityField.C22 == -1)
      return calculateA<real>(mass, radii);
    return mass * radii.mean * radii.mean * (gravityField.C + gravityField.C20 * sqrt(5) - 2 * gravityField.C22 * sqrt(15));
  }
  template <typename real>
  real calculateB(real mass, Radii radii)
  {
    return mass / 5 * (radii.equatorian * radii.equatorian + radii.polar * radii.polar);
  }
  template <typename real>
  real calculateB(real mass, Radii radii, GravityField gravityField)
  {
    if (gravityField.C == -1 && gravityField.C20 == -1 && gravityField.C22 == -1)
      return calculateB<real>(mass, radii);
    return mass * radii.mean * radii.mean * (gravityField.C + gravityField.C20 * sqrt(5) + 2 * gravityField.C22 * sqrt(15));
  }
  template <typename real>
  real calculateC(real mass, Radii radii)
  {
    return mass / 5 * (2 * radii.equatorian * radii.equatorian);
  }
  template <typename real>
  real calculateC(real mass, Radii radii, GravityField gravityField)
  {
    if (gravityField.C == -1 && gravityField.C20 == -1 && gravityField.C22 == -1)
      return calculateC<real>(mass, radii);
    return mass * radii.mean * radii.mean * gravityField.C;
  }
  template <typename real>
  real calculateC20(real mass, Radii radii, real A, real B, real C)
  {
    return -(C - (A + B) / 2) / (mass * radii.mean * radii.mean * sqrt(5));
  }
  template <typename real>
  real calculateC22(real mass, Radii radii, real A, real B)
  {
    return (B - A) / (4 * mass * radii.mean * radii.mean);
  }

  template <typename real>
  glm::dmat3 calculateQuadrupoleTensor(real mass, Radii radii, real A, real B, real C, GravityField field = GravityField())
  {
    double C_20 = field.C20 == -1 ? MomentsMaths::calculateC20<real>(mass, radii, A, B, C) : field.C20;
    double C_22 = field.C22 == -1 ? MomentsMaths::calculateC22<real>(mass, radii, A, B) : field.C22;
    return glm::dmat3(glm::dvec3(-C_20 + 2 * C_22, 0, 0), glm::dvec3(0, -C_20 - 2 * C_22, 0), glm::dvec3(0, 0, 2 * C_20)) *
           mass * radii.mean * radii.mean;
  }
}