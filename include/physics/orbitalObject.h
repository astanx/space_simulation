#pragma once

#include "physics/object.h"
#include "physics/orbit.h"
#include "physics/trail.h"

class Planet;
class Mesh;

struct KeplerElements;

class OrbitalObject : public Object
{
protected:
  Orbit orbit;

  bool useTrail = true;

public:
  OrbitalObject(Object *centralBody, double mu, Radii radii, const KeplerElements &keplerElements, bool useTrail = true);
  virtual ~OrbitalObject() = default;

  Orbit *getOrbit();
  const bool getUseTrail() const;

  void renderTrail();

  virtual std::unique_ptr<Trail> generateTrail();
};