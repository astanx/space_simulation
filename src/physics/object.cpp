#include "physics/constants.h"

#include "physics/object.h"

#include "maths/rk4.h"
#include "maths/constants.h"
#include "maths/momentsMaths.h"

#include <cmath>
#include <iostream>

// Constructor
Object::Object(double mass, Radii radii, TidalParameters tidalParameters, GravityField gravityField, glm::dvec3 position, glm::dvec3 velocity) : inertiaProperties(mass, radii, gravityField), tidalParameters(tidalParameters)
{
  this->mass = mass;
  this->position = position;
  this->velocity = velocity;
  this->quadrupoleTensor = MomentsMaths::calculateQuadrupoleTensor(mass, radii, this->inertiaProperties, gravityField);
  this->acceleration = glm::dvec3(0.0);
  this->orientation = glm::dmat3(1.0);
  this->radii = radii;
  this->angularVelocity = glm::dvec3(0.0);
}

// Public functions
void Object::accelerate(const glm::dvec3 &acc)
{
  this->acceleration += acc;
};

// Getters
glm::dvec3 Object::getVelocity() const
{
  return this->velocity;
}
glm::dvec3 Object::getAngularVelocity() const
{
  return this->angularVelocity;
}
glm::dmat3 Object::getQuadrupoleTensor() const
{
  return this->quadrupoleTensor;
}
glm::dvec3 Object::getAcceleration() const
{
  return this->acceleration;
}
InertiaProperties &Object::getInertiaProperties()
{
  return this->inertiaProperties;
}
const TidalParameters &Object::getTidalParameters()
{
  return this->tidalParameters;
}
double Object::getMass() const
{
  return this->mass;
}
double Object::getMu() const
{
  if (!this->mu)
    return this->mass * G;

  return this->mu;
}
// Setters
void Object::setVelocity(const glm::dvec3 &velocity)
{
  this->velocity = velocity;
}
void Object::setAngularVelocity(const glm::dvec3 &angularVelocity)
{
  this->angularVelocity = angularVelocity;
}
void Object::setAcceleration(const glm::dvec3 &acceleration)
{
  this->acceleration = acceleration;
}
// Public Functions
void Object::applyGravitation(const Object &other)
{
  glm::dvec3 dp = other.position - this->position;
  double distSq = glm::dot(dp, dp);
  if (distSq < EPS)
    return; // Avoid singularity
  double dist = sqrt(distSq);
  glm::dvec3 acc = dp * other.mu / (dist * distSq);
  accelerate(acc);
}
