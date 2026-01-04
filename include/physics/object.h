#pragma once

#include "graphics/model.h"

#include <glm/glm.hpp>

class Object
{
protected:
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 acceleration;
  float mass;
  float radius;

public:
  Object(glm::vec3 position, float mass, float radius, glm::vec3 velocity = glm::vec3(0.f));
  virtual ~Object() = default;

  glm::vec3 getPosition() const;
  glm::vec3& getPositionRef();
  glm::vec3 getVelocity() const;
  float getMass() const;
  float getRadius() const;

  void setVelocity(const glm::vec3 &velocity);
  void setPosition(const glm::vec3 &position);

  void accelerate(const glm::vec3 &acc);
  void move(float dt);

  void applyGravitation(const Object &other);
  static int handleCollisions(Object &object, Object &object2);

  virtual void update(float dt) = 0;
  virtual void render(Shader* shader) = 0;
  virtual Model* getModel() const = 0;
};