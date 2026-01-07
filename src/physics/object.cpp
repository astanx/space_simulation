#include "physics/constants.h"
#include "physics/object.h"
#include "maths/rk4.h"

#include <cmath>
#include <iostream>

// Constructor
Object::Object(double mu, double radius, glm::dvec3 position, glm::dvec3 velocity)
{
  this->mu = mu;
  this->radius = radius;
  this->position = position;
  this->velocity = velocity;
  this->acceleration = glm::dvec3(0.0);
}

// Public functions
void Object::accelerate(const glm::dvec3 &acc)
{
  this->acceleration += acc;
};

// TO-DO Wisdom-Holman
void Object::move(double dt)
{
  this->velocity += this->acceleration * dt;
  this->position += this->velocity * dt;
  this->renderPosition = this->position * VISUAL_SCALE;

  std::cout << "New position: " << renderPosition.x << ' ' << renderPosition.y << ' ' << renderPosition.z << std::endl;

  this->acceleration = glm::dvec3(0.f);
};

// Getters
glm::dvec3 Object::getPosition() const
{
  return this->position;
}
glm::dvec3 Object::getRenderPosition() const
{
  return this->renderPosition;
}
glm::dvec3 Object::getVelocity() const
{
  return this->velocity;
}
glm::dvec3 Object::getAcceleration() const
{
  return this->acceleration;
}
double Object::getMu() const
{
  return this->mu;
}
double Object::getRadius() const
{
  return this->radius;
}

// Setters
void Object::setVelocity(const glm::dvec3 &velocity)
{
  this->velocity = velocity;
}
void Object::setPosition(const glm::dvec3 &position)
{
  this->position = position;
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
