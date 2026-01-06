#include "physics/constants.h"
#include "physics/object.h"
#include "maths/rk4.h"

#include <cmath>
#include <iostream>

// Constructor
Object::Object(glm::dvec3 position, double mass, double radius, glm::dvec3 velocity)
{
  this->position = position;
  this->mass = mass;
  this->radius = radius;
  this->velocity = velocity;
  this->acceleration = glm::dvec3(0.f);
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
double Object::getMass() const
{
  return this->mass;
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
  double force = G * this->mass * other.mass / distSq;
  glm::dvec3 acc = dp * (force / (dist * this->mass));
  accelerate(acc);
}

int Object::handleCollisions(Object &object, Object &object2)
{
  glm::dvec3 dp = object2.getPosition() - object.getPosition();
  double dist = sqrt(pow(dp.x, 2) + pow(dp.y, 2) + pow(dp.z, 2));
  if (dist < EPS)
    return 0;                                          // Avoid singularity
  if (dist < object.getRadius() + object2.getRadius()) // Simple collision response
  {
    glm::dvec3 rv = object2.getVelocity() - object.getVelocity();
    glm::dvec3 np = dp / dist;
    double velAlongNormal = glm::dot(rv, np);
    if (velAlongNormal > 0)
      return 0;    // They are moving apart
    double e = 0.8; // Coefficient of restitution
    double j = -(1 + e) * velAlongNormal / (1 / object.getMass() + 1 / object2.getMass());

    glm::dvec3 penetration = np * ((object.getRadius() + object2.getRadius() - dist) / 2);
    glm::dvec3 impulse = j * np;

    object.setVelocity(object.getVelocity() - (impulse / object.getMass()));
    object.setPosition(object.getPosition() - penetration);

    object2.setVelocity(object2.getVelocity() + (impulse / object2.getMass()));
    object2.setPosition(object2.getPosition() + penetration);
  }
  return 0;
}
