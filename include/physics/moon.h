#pragma once

#include "physics/orbitalObject.h"
#include "physics/structs/hapkeParameters.h"

#include "render/modelSource.h"

class Moon : public OrbitalObject, public ModelSource
{
protected:
  bool useTrail = false;

public:
  Moon(OrbitalObject *centralBody, double mu, Radii radii, const KeplerElements<double> &keplerElements, TidalParameters tidalParameters = TidalParameters(), GravityField gravityField = GravityField());
  ~Moon() = default;
};