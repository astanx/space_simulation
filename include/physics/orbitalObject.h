#pragma once

#include "physics/object.h"
#include "physics/orbit.h"
#include "physics/trail.h"
#include "physics/keplerElements.h"

class Planet;
class Mesh;

class OrbitalObject : public Object
{
protected:
  std::unique_ptr<Orbit> orbit;
  glm::dvec3 orbitalToInertial(double nu);
  glm::dmat3 createR3matrix(double angle);
  glm::dmat3 createR1matrix(double angle);

  glm::dvec3 realToVisualPos(glm::dvec3 pos);

  bool useTrail = true;

public:
  OrbitalObject(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements, bool useTrail = true);
  ~OrbitalObject() = default;

  const Orbit *getOrbit() const;
  const bool getUseTrail() const;

  void keplerDrift(double dt);
  void renderTrail();
  void move(double dt) override;

  virtual std::unique_ptr<Trail> generateTrail();
};