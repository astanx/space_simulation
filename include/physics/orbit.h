#pragma once

#include "physics/structs/keplerElements.h"

#include <glm/glm.hpp>

class Shader;
class Object;
class OrbitalObject;

class Orbit
{
private:
  KeplerElements<double> keplerElements;
  Object *centralBody;

public:
  Orbit(Object *centralBody, const KeplerElements<double> &KeplerElements);
  ~Orbit() = default;

  static glm::dvec3 calculateOrbitalVelocity(const Object *centralBody, OrbitalObject *orbitBody);
  static glm::dvec3 calculateOrbitalVelocity(glm::vec3 bodyPosition, const KeplerElements<double> &bodyElements, glm::vec3 centralPosition, double centralMu);

  Object *getCentralBody() const;

  KeplerElements<double> getKeplerElements() const;
  void updateKeplerElements(KeplerElements<double> newElements);
};