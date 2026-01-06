#pragma once

#include <deque>
#include <glm/glm.hpp>

class Shader;
class Planet;

class Orbit
{
private:
  Planet *centralBody;

  double orbitalPeriod;
  double inclination;
  double longitude;

  std::deque<glm::dvec3> trail;
  size_t maxTrailPoints = 35000;

public:
  Orbit(Planet *centralBody = nullptr, double orbitalPeriod = 0.0, double inclination = 0.0, double longitude = 0.0);
  ~Orbit() = default;

  static glm::dvec3 calculateOrbitalVelocity(const Planet *centralBody, const Planet *orbitBody);

  Planet *getCentralBody();

  double getOrbitalPeriod() const;
  double getInclination() const;
  double getLongitude() const;

  void updateTrail(glm::dvec3 position);
  void renderTrail(Shader *shader);
};