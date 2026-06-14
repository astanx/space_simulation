#pragma once

#include "physics/positionSource.h"
#include "physics/integrators/integratable.h"

#include "physics/structs/radii.h"
#include "physics/structs/inertiaProperties.h"
#include "physics/structs/gravityField.h"
#include "physics/structs/tidalParameters.h"

#include <vector>
#include <glm/glm.hpp>

class Shader;

class Object : public PositionSource, public Integratable
{
protected:
  glm::dvec3 position;
  glm::dvec3 velocity;
  glm::dvec3 acceleration;
  glm::dvec3 angularVelocity;
  glm::dmat3 quadrupoleTensor;
  glm::dmat3 orientation;
  InertiaProperties inertiaProperties;
  TidalParameters tidalParameters;

  double mass;
  double mu;
  Radii radii;

public:
  Object(double mass, Radii radii, TidalParameters tidalParameters = TidalParameters(), GravityField gravityField = GravityField(), glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));
  virtual ~Object() = default;

  glm::dvec3 getPosition() const override;
  glm::dvec3 getVelocity() const;
  glm::dvec3 getAngularVelocity() const;
  glm::dvec3 getAcceleration() const;
  glm::dmat3 getQuadrupoleTensor() const;
  glm::dmat3 getOrientation() const override;
  const TidalParameters &getTidalParameters();
  InertiaProperties &getInertiaProperties();
  double getMass() const;
  double getMu() const;
  double getRadius() const;
  Radii getRadii() const;

  void setVelocity(const glm::dvec3 &velocity);
  void setAngularVelocity(const glm::dvec3 &angularVelocity);
  void setOrientation(const glm::dmat3 &orientation);
  void setAcceleration(const glm::dvec3 &acceleration);
  void setPosition(const glm::dvec3 &position);

  void accelerate(const glm::dvec3 &acc);

  void applyGravitation(const Object &other);
};