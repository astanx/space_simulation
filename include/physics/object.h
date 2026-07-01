#pragma once

#include "physics/transformSource.h"
#include "physics/integrators/integratable.h"

#include "physics/structs/radii.h"
#include "physics/structs/inertiaProperties.h"
#include "physics/structs/gravityField.h"
#include "physics/structs/tidalParameters.h"

#include <vector>
#include <glm/glm.hpp>

class Shader;

class Object : public TransformSource, public Integratable
{
protected:
  glm::dvec3 velocity;
  glm::dvec3 acceleration;
  glm::dvec3 angularVelocity;
  glm::dmat3 quadrupoleTensor;
  InertiaProperties inertiaProperties;
  TidalParameters tidalParameters;

  double mass;
  double mu;

public:
  Object(double mass, Radii radii, TidalParameters tidalParameters = TidalParameters(), GravityField gravityField = GravityField(), glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));
  virtual ~Object() = default;

  glm::dvec3 getVelocity() const;
  glm::dvec3 getAngularVelocity() const;
  glm::dvec3 getAcceleration() const;
  glm::dmat3 getQuadrupoleTensor() const;
  const TidalParameters &getTidalParameters();
  InertiaProperties &getInertiaProperties();
  double getMass() const;
  double getMu() const;

  void setVelocity(const glm::dvec3 &velocity);
  void setAngularVelocity(const glm::dvec3 &angularVelocity);
  void setAcceleration(const glm::dvec3 &acceleration);

  void accelerate(const glm::dvec3 &acc);

  void applyGravitation(const Object &other);
};