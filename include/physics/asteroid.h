#pragma once

#include "physics/orbitalObject.h"
#include "render/renderable.h"

class Model;

class Asteroid : public OrbitalObject, public Renderable
{
protected:
public:
  Asteroid(Object *centralBody, double mu, double radius, const KeplerElements &elements);
  ~Asteroid() = default;

  void update(double dt) override;
  void render(Shader &shader) const override;
  void renderInstanced();
  void addModel(std::unique_ptr<Model> m) override;

  glm::mat4 getModelMatrix();
};