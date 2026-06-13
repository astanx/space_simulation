#pragma once

#include "physics/positionSource.h"
#include "physics/integrators/integratable.h"

#include "physics/structs/radii.h"
#include "physics/structs/inertiaProperties.h"
#include "physics/structs/gravityField.h"

#include <vector>
#include <glm/glm.hpp>

class Shader;

class Object : public PositionSource, public Integratable
{
protected:
  glm::dvec3 position;
  glm::dvec3 velocity;
  glm::dvec3 acceleration;
  // glm::dvec3 angularVelocity;
  // InertiaProperties inertiaProperties;
  // GravityField gravityField;
  double mass;
  double mu;
  Radii radii;

public:
  // Object(double mass, Radii radii, GravityField gravityField, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));
  Object(double mass, Radii radii, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));
  virtual ~Object() = default;

  glm::dvec3 getPosition() const override;
  glm::dvec3 getVelocity() const;
  glm::dvec3 getAcceleration() const;
  double getMass() const;
  double getMu() const;
  double getRadius() const;
  Radii getRadii() const;

  void setVelocity(const glm::dvec3 &velocity);
  void setAcceleration(const glm::dvec3 &acceleration);
  void setPosition(const glm::dvec3 &position);

  void accelerate(const glm::dvec3 &acc);

  void applyGravitation(const Object &other);
  static int handleCollisions(Object &object, Object &object2);
};