#include "physics/orbitalObject.h"

#include "physics/constants.h"

#include "maths/orbitalMaths.h"

#include "graphics/vertex.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Constructor
OrbitalObject::OrbitalObject(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements, bool useTrail) : Object(mu / G, radius), orbit(centralBody, keplerElements)
{
  this->mu = mu;
  this->useTrail = useTrail;
  this->position = OrbitalMaths::orbitalToInertial(this->orbit.getKeplerElements());
  this->position += centralBody->getPosition();

  this->velocity = this->orbit.calculateOrbitalVelocity(centralBody, this);
  this->velocity += this->orbit.getCentralBody()->getVelocity();
}

// Public functions
Orbit *OrbitalObject::getOrbit()
{
  return &this->orbit;
}

const bool OrbitalObject::getUseTrail() const
{
  return this->useTrail;
}

std::unique_ptr<Trail> OrbitalObject::generateTrail()
{
  std::vector<glm::dvec3> trailVec;
  for (double nu = 0; nu < 2 * M_PI; nu += 0.01)
  {
    glm::dvec3 pos = OrbitalMaths::orbitalToInertial(this->orbit.getKeplerElements(), nu);
    pos += this->orbit.getCentralBody()->getPosition();

    trailVec.push_back(pos);
  }

  return std::make_unique<Trail>(trailVec);
}
