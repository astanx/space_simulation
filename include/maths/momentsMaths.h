#pragma once

#include "physics/structs/gravityField.h"

#include <glm/glm.hpp>
#include <vector>

struct Radii;
struct InertiaProperties;
class Object;

namespace MomentsMaths
{
  double calculateA(double mass, Radii radii);
  double calculateA(double mass, Radii radii, GravityField gravityField);
  double calculateB(double mass, Radii radii);
  double calculateB(double mass, Radii radii, GravityField gravityField);
  double calculateC(double mass, Radii radii);
  double calculateC(double mass, Radii radii, GravityField gravityField);
  double calculateC20(double mass, Radii radii, InertiaProperties properties);
  double calculateC22(double mass, Radii radii, InertiaProperties properties);

  glm::dmat3 calculateQuadrupoleTensor(double mass, Radii radii, InertiaProperties properties, GravityField field = GravityField());
  glm::dvec3 calculateTorque(Object *object, const std::vector<Object *> &bodies);

  glm::dvec3 calculateGravitationalTorque(Object *object, const Object* body);
  glm::dvec3 calculateTidalTorque(Object *object, const Object* body);
}