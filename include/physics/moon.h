#pragma once

#include "physics/orbitalObject.h"
#include "graphics/renderable.h"

class Model;

class Moon : public OrbitalObject, public Renderable
{
protected:
  bool useTrail = false;

public:
  Moon(OrbitalObject *centralBody, double mu, double radius, const KeplerElements &keplerElements);
  ~Moon() = default;

  void update(double dt) override;
  void render(Shader *shader) override;
  void addModel(std::unique_ptr<Model> m) override;
  std::unique_ptr<Trail> generateTrail() override;
};