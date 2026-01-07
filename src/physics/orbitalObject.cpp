#include "physics/orbitalObject.h"

#include <iostream>

// Constructor
OrbitalObject::OrbitalObject(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements) : Object(mu, radius)
{
  std::cout << "A: " << keplerElements.a << std::endl;
  this->position = glm::dvec3(keplerElements.a, 0.0, 0.0) + centralBody->getPosition();
  this->orbit = std::make_unique<Orbit>(centralBody, keplerElements);
  if (this->orbit)
  {
    this->velocity = this->orbit->calculateOrbitalVelocity(centralBody, this);
    this->velocity += this->orbit->getCentralBody()->getVelocity();
  }
}

const Orbit *OrbitalObject::getOrbit() const
{
  return this->orbit.get();
}