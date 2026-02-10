#pragma once

#include "physics/object.h"
#include "physics/orbit.h"
#include "physics/trail.h"

class Planet;
class Mesh;

struct KeplerElements;

class OrbitalObject : public Object
{
protected:
  std::unique_ptr<Orbit> orbit;
  glm::dvec3 orbitalToInertial(double nu = -1.0);
  glm::dmat3 createR3matrix(double angle);
  glm::dmat3 createR1matrix(double angle);
  double calculateEccentricAnomaly(double M, KeplerElements keplerElements);

  bool useTrail = true;

  void keplerDrift(double dt);
  void kick(const std::vector<Object *> &bodies, double dt) override;

public:
  OrbitalObject(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements, bool useTrail = true);
  ~OrbitalObject() = default;

  const Orbit *getOrbit() const;
  const bool getUseTrail() const;

  virtual void drift(double dt) override;
  virtual void halfKick(const std::vector<Object *> &bodies, double dt) override;
  void renderTrail();
  // void move(double dt) override;

  virtual std::unique_ptr<Trail> generateTrail();
};