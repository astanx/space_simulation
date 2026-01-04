#pragma once

#include "physics/object.h"

#include <deque>

class Model;

class Planet : public Object
{
protected:
  std::unique_ptr<Model> model;
  Planet *centralBody;
  float orbitalPeriod;

  std::deque<glm::vec3> trail;
  size_t maxTrailPoints = 35000;
  glm::vec3 trailColor;

public:
  Planet(glm::vec3 position, float mass, float radius, std::unique_ptr<Model> model, glm::vec3 velocity = glm::vec3(0.f), Planet *centralBody = nullptr, float orbitalPeriod = 0.f, glm::vec3 trailColor = glm::vec3(1.f));
  ~Planet() = default;

  static glm::vec3 calculateOrbitalSpeed(const Planet *centralBody, const Planet *orbitBody);

  float getOrbitalPeriod() const;

  void renderTrail(Shader *shader);

  void update(float dt) override;
  void render(Shader *shader) override;
  Model *getModel() const override;
};