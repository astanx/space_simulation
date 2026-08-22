#pragma once

#include "physics/structs/keplerElements.h"

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

  static glm::dvec3 calculateOrbitalVelocity(const Object *centralBody, OrbitalObject *orbitBody);
  static glm::dvec3 calculateOrbitalVelocity(glm::vec3 bodyPosition, KeplerElements bodyElements, glm::vec3 centralPosition, double centralMu);

  Object *getCentralBody() const;

  KeplerElements getKeplerElements() const;
  void updateKeplerElements(KeplerElements newElements);
};