#include "physics/constants.h"
#include "physics/object.h"
#include "maths/rk4.h"

#include <cmath>
#include <iostream>

// Private functions
glm::dvec3 Object::realToVisualPos(glm::dvec3 pos)
{
  return glm::dvec3(
             pos.x,
             -pos.z, // Z → Y
             pos.y   // Y → -Z
             ) *
         VISUAL_SCALE;
  // return this->position * VISUAL_SCALE;
}

void Object::kick(const std::vector<Object *> &bodies, double dt)
{
  this->acceleration = glm::dvec3(0.0);

  for (auto *other : bodies)
  {
    if (other == this)
      continue;

    this->applyGravitation(*other);
  }

  this->velocity += dt * this->acceleration; // kick
}

// Constructor
Object::Object(double mass, double radius, glm::dvec3 position, glm::dvec3 velocity)
{
  this->mass = mass;
  this->radius = radius;
  this->position = position;
  this->renderPosition = realToVisualPos(position);
  this->velocity = velocity;
  this->acceleration = glm::dvec3(0.0);
}

// Public functions
void Object::accelerate(const glm::dvec3 &acc)
{
  this->acceleration += acc;
};

void Object::move(double dt)
{
  this->velocity += this->acceleration * dt;
  this->position += this->velocity * dt;
  this->renderPosition = this->realToVisualPos(this->position);

  this->acceleration = glm::dvec3(0.f);
};

void Object::drift(double dt)
{
  this->position += this->velocity * dt;
  this->renderPosition = this->realToVisualPos(this->position);
}

void Object::halfKick(const std::vector<Object *> &bodies, double dt)
{
  this->kick(bodies, dt * 0.5);
}

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
double Object::getMass() const
{
  return this->mass;
}
double Object::getMu() const
{
  if (!this->mu)
  {
    return this->mass * G;
  }
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
  this->renderPosition = realToVisualPos(position);
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
