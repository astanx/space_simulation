#include "physics/constants.h"
#include "physics/object.h"

#include <cmath>
#include <iostream>

// Constructor
Object::Object(glm::vec3 position, float mass, float radius, glm::vec3 velocity)
{
  this->position = position;
  this->mass = mass;
  this->radius = radius;
  this->velocity = velocity;
  // acceleration = {0.0, -9.81}; // Initial acceleration due to gravity
  this->acceleration = glm::vec3(0.f);
}

// Public functions
void Object::accelerate(const glm::vec3 &acc)
{
  this->acceleration += acc;
};

void Object::move(float dt)
{
  this->velocity += this->acceleration * dt;
  this->position += velocity * dt;
  std::cout << "New position: " << position.x << ' ' << position.y << ' ' << position.z << std::endl; 

  // acceleration = {0.0, -9.81}; // Reset acceleration after move
  this->acceleration = glm::vec3(0.f);
  // if (position[1] - radius < -1.0)
  // { // Ground collision
  //   position[1] = -1.0 + radius;
  //   velocity[1] = -velocity[1] * 0.6; // Simple bounce with damping
  // }
  // if (position[1] + radius > 1.0)
  // { // Ceiling collision
  //   position[1] = 1.0 - radius;
  //   velocity[1] = -velocity[1] * 0.6;
  // }
  // if (position[0] - radius < -1.0)
  // { // Left wall collision
  //   position[0] = -1.0 + radius;
  //   velocity[0] = -velocity[0] * 0.6;
  // }
  // if (position[0] + radius > 1.0)
  // { // Right wall collision
  //   position[0] = 1.0 - radius;
  //   velocity[0] = -velocity[0] * 0.6;
  // }
}
// Getters
glm::vec3 Object::getPosition() const
{
  return this->position;
}
glm::vec3 &Object::getPositionRef()
{
  return this->position;
}
glm::vec3 Object::getVelocity() const
{
  return this->velocity;
}
float Object::getMass() const
{
  return this->mass;
}
float Object::getRadius() const
{
  return this->radius;
}

// Setters
void Object::setVelocity(const glm::vec3 &velocity)
{
  this->velocity = velocity;
}
void Object::setPosition(const glm::vec3 &position)
{
  this->position = position;
}

// Public Functions
void Object::applyGravitation(const Object &other)
{
  glm::vec3 dp = other.position - this->position;
  float distSq = glm::dot(dp, dp);
  if (distSq < EPS)
    return; // Avoid singularity
  float dist = sqrt(distSq);
  float force = G * this->mass * other.mass / distSq;
  glm::vec3 acc = dp * (force / (dist * this->mass));

  accelerate(acc);
}

int Object::handleCollisions(Object &object, Object &object2)
{
  glm::vec3 dp = object2.getPosition() - object.getPosition();
  float dist = sqrt(pow(dp.x, 2) + pow(dp.y, 2) + pow(dp.z, 2));
  if (dist < EPS)
    return 0;                                          // Avoid singularity
  if (dist < object.getRadius() + object2.getRadius()) // Simple collision response
  {
    glm::vec3 rv = object2.getVelocity() - object.getVelocity();
    glm::vec3 np = dp / dist;
    float velAlongNormal = glm::dot(rv, np);
    if (velAlongNormal > 0)
      return 0;    // They are moving apart
    float e = 0.8; // Coefficient of restitution
    float j = -(1 + e) * velAlongNormal / (1 / object.getMass() + 1 / object2.getMass());

    glm::vec3 penetration = np * ((object.getRadius() + object2.getRadius() - dist) / 2);
    glm::vec3 impulse = j * np;

    object.setVelocity(object.getVelocity() - (impulse / object.getMass()));
    object.setPosition(object.getPosition() - penetration);

    object2.setVelocity(object2.getVelocity() + (impulse / object2.getMass()));
    object2.setPosition(object2.getPosition() + penetration);
  }
  return 0;
}
