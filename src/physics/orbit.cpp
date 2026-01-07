#include "physics/orbit.h"
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
Object *Orbit::getCentralBody()
{
  return this->centralBody;
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
glm::dvec3 Orbit::calculateOrbitalVelocity(const Object *centralBody, const OrbitalObject *orbitBody)
{
  if (!centralBody || !orbitBody)
    throw "ERROR:ORBIT:CALCULATE_VELOCITY:NO_BODY";

  glm::dvec3 normal(0.0);
  glm::dvec3 velocity(0.0);

  const Orbit *orbit = orbitBody->getOrbit();

  KeplerElements elements = orbit->getKeplerElements();

  normal.x = sin(elements.i) * sin(elements.Omega);
  normal.y = cos(elements.i);
  normal.z = sin(elements.i) * cos(elements.Omega);

  std::cout<< "Central: " << centralBody->getPosition().x << " " << " " << orbitBody->getPosition().x << std::endl;

  glm::dvec3 dp = centralBody->getPosition() - orbitBody->getPosition();

  double r = glm::length(dp);
  if (r < EPS)
  {
    std::cerr << "ERROR:ORBIT:CALCULATE_VELOCITY: r is too small" << std::endl;
    return glm::dvec3(0.0);
  }

  double speed = sqrt(centralBody->getMu() * (2 / r - 1 / elements.a)); // Vis-viva equation

  glm::dvec3 v_dir = glm::normalize(glm::cross(normal, dp));
  velocity = speed * v_dir;
  std::cout << "Orbital speed: " << velocity.x << ' ' << velocity.y << ' ' << velocity.z << std::endl;
  return velocity;
}
