#include "physics/orbit.h"
#include "physics/constants.h"
#include "physics/planet.h"
#include "graphics/shader.h"
#include "graphics/vertex.h"
#include "graphics/mesh.h"

#include <iostream>

// Constructor / Destructor
Orbit::Orbit(Planet *centralBody, double orbitalPeriod, double inclination, double longitude)
{
  this->centralBody = centralBody;
  this->orbitalPeriod = orbitalPeriod;
  this->inclination = inclination;
  this->longitude = longitude;
}

// Public functions
double Orbit::getOrbitalPeriod() const
{
  return this->orbitalPeriod;
}
Planet *Orbit::getCentralBody()
{
  return this->centralBody;
}
double Orbit::getInclination() const
{
  return this->inclination;
}
double Orbit::getLongitude() const
{
  return this->longitude;
}

void Orbit::renderTrail(Shader *shader)
{
  if (trail.size() < 2)
    return;

  std::vector<Vertex> vertices;
  for (auto &p : trail)
  {
    Vertex v{};
    v.position = p;
    v.color = glm::vec3(1.f);
    v.texcoord = glm::vec2(0.0f);
    v.normal = glm::vec3(0.0f);

    vertices.push_back(v);
  }
  Mesh mesh(vertices.data(), vertices.size(), nullptr, 0, GL_LINE_STRIP);

  shader->set1i(0, "useModelMatrix");
  shader->set1i(1, "isTexture");

  mesh.render(shader);
}

void Orbit::updateTrail(glm::dvec3 position)
{
  this->trail.push_back(position);
  if (this->trail.size() > this->maxTrailPoints)
    trail.pop_front();
}

// Static functions
glm::dvec3 Orbit::calculateOrbitalVelocity(const Planet *centralBody, const Planet *orbitBody)
{
  if (!centralBody || !orbitBody)
    throw "ERROR:ORBIT:CALCULATE_VELOCITY:NO_BODY";
  
  glm::dvec3 normal(0.0);
  glm::dvec3 velocity(0.0);

  Orbit* orbit = orbitBody->getOrbit();

  double i = glm::radians(orbit->getInclination());
  double longitude = glm::radians(orbit->getLongitude());

  normal.x = sin(i) * sin(longitude);
  normal.y = cos(i);
  normal.z = sin(i) * cos(longitude);

  glm::dvec3 dp = centralBody->getPosition() - orbitBody->getPosition();

  double a;                                                                                                    // semi-major axis
  a = cbrt(pow(orbitBody->getOrbit()->getOrbitalPeriod(), 2) * G * centralBody->getMass() / 4 / pow(M_PI, 2)); // Kepler`s third law
  double r = glm::length(dp);
  double speed = sqrt(G * centralBody->getMass() * (2 / r - 1 / a)); // Vis-viva equation

  glm::dvec3 v_dir = glm::normalize(glm::cross(normal, dp));
  velocity = speed * v_dir;
  std::cout << "Orbital speed: " << velocity.x << ' ' << velocity.y << ' ' << velocity.z << std::endl;
  return velocity;
}
