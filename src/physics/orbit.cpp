#include "physics/orbit.h"

#include "debug/logger.h"

#include "physics/constants.h"
#include "physics/planet.h"

#include "maths/constants.h"

#include "graphics/shader.h"
#include "graphics/vertex.h"
#include "graphics/mesh.h"

#include <iostream>

// Constructor / Destructor
Orbit::Orbit(Object *centralBody, const KeplerElements &keplerElements) : keplerElements(keplerElements)
{
  this->centralBody = centralBody;
}

// Public functions
KeplerElements Orbit::getKeplerElements() const
{
  return this->keplerElements;
}
void Orbit::updateKeplerElements(KeplerElements newElements)
{
  this->keplerElements = newElements;
  // this->keplerElements.calculateMeanMotion(this->centralBody->getMu());
}
Object *Orbit::getCentralBody() const
{
  return this->centralBody;
}

// Static functions
glm::dvec3 Orbit::calculateOrbitalVelocity(glm::vec3 bodyPosition, KeplerElements bodyElements, glm::vec3 centralPosition, double centralMu)
{
  glm::dvec3 normal(0.0);
  glm::dvec3 velocity(0.0);

  normal.x = sin(bodyElements.i) * sin(bodyElements.Omega);
  normal.y = -sin(bodyElements.i) * cos(bodyElements.Omega);
  normal.z = cos(bodyElements.i);

  glm::dvec3 dp = centralPosition - bodyPosition;

  double r = glm::length(dp);
  if (r < EPS)
  {
    Logger::logError("Orbit", "r is too small, setting velocity to 0");
    return glm::dvec3(0.0);
  }

  double speed = sqrt(centralMu * (2 / r - 1 / bodyElements.a)); // Vis-viva equation

  glm::dvec3 v_dir = glm::normalize(glm::cross(normal, dp));
  velocity = speed * v_dir;
  // std::cout << "Orbital speed: " << velocity.x << ' ' << velocity.y << ' ' << velocity.z << std::endl;
  return velocity;
}

glm::dvec3 Orbit::calculateOrbitalVelocity(const Object *centralBody, OrbitalObject *orbitBody)
{
  if (!centralBody || !orbitBody)
    Logger::logFatal("Orbit", "No body to calculate velocity");

  return Orbit::calculateOrbitalVelocity(orbitBody->getPosition(), orbitBody->getOrbit()->getKeplerElements(), centralBody->getPosition(), centralBody->getMu());
}
