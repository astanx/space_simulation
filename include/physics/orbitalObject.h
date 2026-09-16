#pragma once

#include "physics/object.h"
#include "physics/orbit.h"
#include "physics/trail.h"

#include "physics/structs/keplerElements.h"

class Mesh;

class OrbitalObject : public Object
{
protected:
  Orbit orbit;

public:
  OrbitalObject(Object *centralBody, double mu, Radii radii, const KeplerElements<double> &keplerElements, TidalParameters tidalParameters = TidalParameters(), GravityField gravityField = GravityField());
  virtual ~OrbitalObject() = default;

  Orbit *getOrbit();
};