#pragma once

#include "physics/orbitalObject.h"
#include "physics/structs/hapkeParameters.h"

#include "render/modelSource.h"

class Model;

class Moon : public OrbitalObject, public ModelSource
{
protected:
  bool useTrail = false;
  HapkeParameters hapkeParameters;

public:
  Moon(OrbitalObject *centralBody, double mu, Radii radii, const KeplerElements &keplerElements, const HapkeParameters &hapkeParameters, GravityField gravityField = GravityField());
  ~Moon() = default;

  void sendHapkeParametersToShader(Shader &shader) const;
};