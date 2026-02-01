#pragma once

#include <glm/glm.hpp>

class Shader;

class Object
{
protected:
  glm::dvec3 position;
  glm::dvec3 renderPosition;
  glm::dvec3 velocity;
  glm::dvec3 acceleration;
  double mass;
  double mu;
  double radius;

  glm::dvec3 realToVisualPos(glm::dvec3 pos);

public:
  Object(double mass, double radius, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));
  virtual ~Object() = default;

  glm::dvec3 getPosition() const;
  glm::dvec3 getRenderPosition() const;
  glm::dvec3 getVelocity() const;
  glm::dvec3 getAcceleration() const;
  double getMass() const;
  double getMu() const;
  double getRadius() const;

  void setVelocity(const glm::dvec3 &velocity);
  void setPosition(const glm::dvec3 &position);

  void accelerate(const glm::dvec3 &acc);
  virtual void move(double dt);

  void applyGravitation(const Object &other);
  static int handleCollisions(Object &object, Object &object2);

  virtual void update(double dt) = 0;
  virtual void render(Shader *shader) = 0;
};