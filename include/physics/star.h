#pragma once

#include "physics/object.h"
#include "graphics/renderable.h"

class Model;

class Star : public Object, public Renderable
{
protected:
public:
  Star(double mu, double radius, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));
  ~Star() = default;

  void update(double dt) override;
  void render(Shader *shader) override;
  void addModel(std::unique_ptr<Model> m) override;
};