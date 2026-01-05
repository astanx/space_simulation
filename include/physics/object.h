#pragma once

#include "graphics/model.h"

#include <glm/glm.hpp>

class Object
{
protected:
  glm::dvec3 position;
  glm::dvec3 velocity;
  glm::dvec3 acceleration;
  double mass;
  double radius;

public:
  Object(glm::vec3 position, double mass, double radius, glm::vec3 velocity = glm::vec3(0.f));
  virtual ~Object() = default;

  glm::dvec3 getPosition() const;
  glm::dvec3& getPositionRef();
  glm::dvec3 getVelocity() const;
  double getMass() const;
  double getRadius() const;

  void setVelocity(const glm::dvec3 &velocity);
  void setPosition(const glm::dvec3 &position);

  void accelerate(const glm::dvec3 &acc);
  void move(double dt);

  void applyGravitation(const Object &other);
  static int handleCollisions(Object &object, Object &object2);

  virtual void update(double dt) = 0;
  virtual void render(Shader* shader) = 0;
  virtual Model* getModel() const = 0;
};