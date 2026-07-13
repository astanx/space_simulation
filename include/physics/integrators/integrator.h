#pragma once

#include <vector>

class Integratable;
class Object;
class OrbitalObject;
class System;

class Integrator
{
protected:
  void splitObjectsSystems(std::vector<Integratable *> &objects, std::vector<Object *> &objectPointers, std::vector<System *> &systemPointers);
  void splitObjectsSystems(std::vector<Integratable *> &objects, std::vector<Object *> &objectPointers, std::vector<OrbitalObject *> &orbitalObjectsPointers, std::vector<System *> &systemPointers);

public:
  Integrator() = default;
  virtual ~Integrator() = default;
};