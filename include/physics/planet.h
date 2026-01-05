#pragma once

#include "physics/object.h"

class Model;
class Orbit;

class Planet : public Object
{
protected:
  std::unique_ptr<Model> model;
  std::unique_ptr<Orbit> orbit;

public:
  Planet(glm::dvec3 position, double mass, double radius, std::unique_ptr<Model> model, glm::dvec3 velocity = glm::vec3(0.f), std::unique_ptr<Orbit> orbit = nullptr);
  ~Planet() = default;

  void update(double dt) override;
  void render(Shader *shader) override;
  Model *getModel() const override;
  Orbit *getOrbit() const;
};