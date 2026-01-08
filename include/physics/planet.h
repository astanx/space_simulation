#pragma once

#include "physics/orbitalObject.h"
#include "physics/keplerElements.h"
#include "physics/moon.h"
#include "graphics/renderable.h"

class Model;

class Planet : public OrbitalObject, public Renderable
{
protected:
  std::vector<std::unique_ptr<Moon>> moons;

public:
  Planet(Object* centralBody, double mu, double radius, const KeplerElements& keplerElements);
  ~Planet() = default;

  void addMoon(std::unique_ptr<Moon> moon);

  void update(double dt) override;
  void render(Shader *shader) override;
  void addModel(std::unique_ptr<Model> m) override;
};