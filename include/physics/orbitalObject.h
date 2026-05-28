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

  // IAS15 integrator
  void accelerationBinominal();

public:
  OrbitalObject(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements, bool useTrail = true);
  virtual ~OrbitalObject() = default;

  Orbit *getOrbit();
  const bool getUseTrail() const;

  void renderTrail();

  virtual std::unique_ptr<Trail> generateTrail();
};