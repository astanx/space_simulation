#pragma once

#include "physics/integrators/integrator.h"

class Object;
class OrbitalObject;
class Integratable;

class WisdomHolmanIntegrator : public Integrator
{
protected:
  void halfKick(Object *object, const std::vector<Object *> &objects, double dt);
  void drift(Object *object, const std::vector<Object *> &objects, double dt);

  void keplerDrift(OrbitalObject *object, double dt);

public:
  WisdomHolmanIntegrator() = default;
  ~WisdomHolmanIntegrator() = default;

  void step(std::vector<Integratable *> &objects, double dt) override;
};