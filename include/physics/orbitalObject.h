#pragma once

#include "physics/object.h"
#include "physics/orbit.h"
#include "physics/keplerElements.h"

class Planet;
class Mesh;

class OrbitalObject : public Object
{
protected:
  Object *centralBody;
  std::unique_ptr<Orbit> orbit;

  KeplerElements keplerElements;

  std::unique_ptr<Mesh> trail;

  void generateTrail(const KeplerElements &keplerElements);
  glm::dvec3 orbitalToInertial(double nu, const KeplerElements &keplerElements);
  glm::dmat3 createR3matrix(double angle);
  glm::dmat3 createR1matrix(double angle);

  glm::dvec3 realToVisualPos(glm::dvec3 pos);

public:
  OrbitalObject(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements);
  ~OrbitalObject() = default;

  const Orbit *getOrbit() const;

  glm::dvec3 keplerDrift(double dt);

  void renderTrail(Shader *shader);
  void move(double dt) override;
};