#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Radii;
struct GravityField;
class Object;

namespace MomentsMaths
{
  double calculateA(double mass, Radii radii);
  double calculateA(double mass, Radii radii, GravityField gravityField);
  double calculateB(double mass, Radii radii);
  double calculateB(double mass, Radii radii, GravityField gravityField);
  double calculateC(double mass, Radii radii);
  double calculateC(double mass, Radii radii, GravityField gravityField);
  double calculateC20(double mass, Radii radii);
  double calculateC22(double mass, Radii radii);

  glm::dmat3 calculateQuadrupoleTensor(double mass, Radii radii, double C20 = -1, double C22 = -1);
  glm::dvec3 calculateTorque(std::vector<Object> &bodies, glm::dmat3 quadropleTensor, glm::dvec3 position);
}