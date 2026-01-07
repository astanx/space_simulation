#pragma once

#include "physics/orbitalObject.h"
#include "graphics/renderable.h"

class Model;

class Moon : public OrbitalObject, public Renderable
{
protected:
public:
  Moon(Object *centralBody, double mu, double radius, const KeplerElements& keplerElements);
  ~Moon() = default;

  void update(double dt) override;
  void render(Shader *shader) override;
  void addModel(std::unique_ptr<Model> m) override;
};