#pragma once

#include "physics/keplerElements.h"

#include <deque>
#include <glm/glm.hpp>

class Shader;
class Object;
class OrbitalObject;

class Orbit
{
private:
  KeplerElements keplerElements;
  Object *centralBody;

  std::deque<glm::dvec3> trail;
  size_t maxTrailPoints = 35000;

public:
  Orbit(Object *centralBody, const KeplerElements &KeplerElements);
  ~Orbit() = default;

  static glm::dvec3 calculateOrbitalVelocity(const Object *centralBody, const OrbitalObject *orbitBody);

  Object *getCentralBody();

  KeplerElements getKeplerElements() const;

  double getInclination() const;
  double getSemiMajorAxis() const;

  void updateTrail(glm::dvec3 position);
  void renderTrail(Shader *shader);
};