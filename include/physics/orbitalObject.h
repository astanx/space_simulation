#pragma once

#include "physics/object.h"
#include "physics/orbit.h"
#include "physics/keplerElements.h"

class Planet;

class OrbitalObject : public Object
{
protected:
  Object* centralBody;
  std::unique_ptr<Orbit> orbit;

public:
  OrbitalObject(Object *centralBody, double mu, double radius, const KeplerElements& keplerElements);
  ~OrbitalObject() = default;

  const Orbit *getOrbit() const;
};