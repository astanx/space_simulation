#include "physics/orbitalObject.h"

#include "maths/orbitalMaths.h"

#include <iostream>

// Constructor
OrbitalObject::OrbitalObject(Object *centralBody, double mu, Radii radii, const KeplerElements<double> &keplerElements, TidalParameters tidalParameters, GravityField gravityField) : Object(mu / G, radii, tidalParameters, gravityField), orbit(centralBody, keplerElements)
{
  this->mu = mu;

  double E = OrbitalMaths::calculateEccentricAnomaly(keplerElements.m, keplerElements.e);

  this->position.x = keplerElements.a * (cos(E) - keplerElements.e);
  this->position.y = keplerElements.a * sqrt(1 - (keplerElements.e * keplerElements.e)) * sin(E);

  double r = keplerElements.a * (1 - keplerElements.e * cos(E));

  this->velocity.x = -sqrt(centralBody->getMu() * keplerElements.a) / r * sin(E);
  this->velocity.y = sqrt(centralBody->getMu() * keplerElements.a * (1 - (keplerElements.e * keplerElements.e))) / r * cos(E);

  glm::dmat3 R = OrbitalMaths::createR3matrix<double>(keplerElements.Omega) * OrbitalMaths::createR1matrix<double>(keplerElements.i) * OrbitalMaths::createR3matrix<double>(keplerElements.omega);

  this->velocity = R * this->velocity + centralBody->getVelocity();
  this->position = R * this->position + centralBody->getPosition();
}

// Public functions
Orbit *OrbitalObject::getOrbit()
{
  return &this->orbit;
}