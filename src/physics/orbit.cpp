#include "physics/orbit.h"

#include "debug/logger.h"

#include "physics/constants.h"
#include "physics/planet.h"

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
}
Object *Orbit::getCentralBody()
{
  return this->centralBody;
}

// Static functions
glm::dvec3 Orbit::calculateOrbitalVelocity(const Object *centralBody, const OrbitalObject *orbitBody)
{
  if (!centralBody || !orbitBody)
    throw std::runtime_error("[Orbit] RUNTIME ERROR: no body to calculate velocity");

  glm::dvec3 normal(0.0);
  glm::dvec3 velocity(0.0);

  const Orbit *orbit = orbitBody->getOrbit();

  KeplerElements elements = orbit->getKeplerElements();

  normal.x = sin(elements.i) * sin(elements.Omega);
  normal.y = -sin(elements.i) * cos(elements.Omega);
  normal.z = cos(elements.i);

  glm::dvec3 dp = centralBody->getPosition() - orbitBody->getPosition();

  double r = glm::length(dp);
  if (r < EPS)
  {
    Logger::logError("Orbit", "r is too small, setting velocity to 0");
    return glm::dvec3(0.0);
  }

  double speed = sqrt(centralBody->getMu() * (2 / r - 1 / elements.a)); // Vis-viva equation

  glm::dvec3 v_dir = glm::normalize(glm::cross(normal, dp));
  velocity = speed * v_dir;
  // std::cout << "Orbital speed: " << velocity.x << ' ' << velocity.y << ' ' << velocity.z << std::endl;
  return velocity;
}
