#pragma once

#include "physics/structs/radii.h"
#include "physics/structs/gravityField.h"

#include "maths/momentsMaths.h"

struct InertiaProperties
{
  double A;
  double B;
  double C;

  InertiaProperties(double mass, Radii radii, GravityField field = GravityField())
  {
    this->A = MomentsMaths::calculateA(mass, radii, field);
    this->B = MomentsMaths::calculateB(mass, radii, field);
    this->C = MomentsMaths::calculateC(mass, radii, field);
  }

  glm::dmat3 getInertiaTensor()
  {
    glm::dmat3 mat;
    mat[0][0] = A;
    mat[1][1] = B;
    mat[2][2] = C;

    return mat;
  }
};