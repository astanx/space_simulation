#pragma once

#include "physics/orbitalObject.h"
#include "physics/structs/hapkeParameters.h"

#include "render/renderable.h"

class Model;

class Moon : public OrbitalObject, public Renderable
{
protected:
  bool useTrail = false;
  HapkeParameters hapkeParameters;

public:
  Moon(OrbitalObject *centralBody, double mu, double radius, const KeplerElements &keplerElements, const HapkeParameters &hapkeParameters);
  ~Moon() = default;

  void update(double dt) override;
  void render(Shader &shader) const override;
  void addModel(std::unique_ptr<Model> m) override;
  std::unique_ptr<Trail> generateTrail() override;

  void sendHapkeParametersToShader(Shader &shader) const;
};