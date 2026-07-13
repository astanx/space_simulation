#pragma once

#include "physics/integrators/integratorCPU.h"

class Object;
class OrbitalObject;
class Integratable;

class WisdomHolmanIntegratorCPU : public IntegratorCPU
{
protected:
  void halfKickLinear(Object *object, const std::vector<Object *> &objects, double dt);
  void driftLinear(Object *object, double dt);

  void halfKickAngular(Object *object, const std::vector<Object *> &objects, double dt);
  void driftAngular(Object *object, double dt);

  void halfKick(Object *object, const std::vector<Object *> &objects, double dt);
  void drift(Object *object, double dt);

  void keplerDrift(OrbitalObject *object, double dt);

public:
  WisdomHolmanIntegratorCPU() = default;
  ~WisdomHolmanIntegratorCPU() = default;

  void step(std::vector<Integratable *> &objects, double dt) override;
};