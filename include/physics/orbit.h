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

  static glm::vec3 calculateOrbitalVelocity(const Planet *centralBody, const Planet *orbitBody);

  double getOrbitalPeriod() const;
  Planet *getCentralBody();
  void updateTrail(glm::dvec3 position);

  void renderTrail(Shader *shader);
};