#pragma once

#include "physics/object.h"

class Model;

class Planet : public Object
{
protected:
  std::unique_ptr<Model> model;
  Planet* centralBody;
  float orbitalPeriod;

public:
  Planet(glm::vec3 position, float mass, float radius, std::unique_ptr<Model> model, glm::vec3 velocity = glm::vec3(0.f), Planet* centralBody = nullptr, float orbitalPeriod = 0.f);
  ~Planet() = default;

  static glm::vec3 calculateOrbitalSpeed(const Planet* centralBody, const Planet* orbitBody);

  void update(float dt) override;
  void render(Shader* shader) override;
  Model* getModel() const override;
};