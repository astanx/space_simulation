#pragma once

#include "physics/integrators/integrator.h"

class Object;
class OrbitalObject;
class Integratable;

class WisdomHolmanIntegrator : public Integrator
{
protected:
  void halfKickLinear(Object *object, const std::vector<Object *> &objects, double dt);
  void driftLinear(Object *object, const std::vector<Object *> &objects, double dt);

  void halfKickAngular(Object *object, const std::vector<Object *> &objects, double dt);
  void driftAngular(Object *object, const std::vector<Object *> &objects, double dt);

  void halfKick(Object *object, const std::vector<Object *> &objects, double dt);
  void drift(Object *object, const std::vector<Object *> &objects, double dt);

  void keplerDrift(OrbitalObject *object, double dt);

public:
  WisdomHolmanIntegrator() = default;
  ~WisdomHolmanIntegrator() = default;

  void step(std::vector<Integratable *> &objects, double dt) override;
};