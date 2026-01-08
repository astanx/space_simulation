#pragma once

#include "physics/keplerElements.h"

#include <glm/glm.hpp>

class Shader;
class Object;
class OrbitalObject;

class Orbit
{
private:
  KeplerElements keplerElements;
  Object *centralBody;
  
public:
  Orbit(Object *centralBody, const KeplerElements &KeplerElements);
  ~Orbit() = default;

  static glm::dvec3 calculateOrbitalVelocity(const Object *centralBody, const OrbitalObject *orbitBody);

  Object *getCentralBody();

  KeplerElements getKeplerElements() const;
  void updateKeplerElements(KeplerElements newElements);

  double getInclination() const;
  double getSemiMajorAxis() const;
};