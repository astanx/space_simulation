#include "physics/orbitalObject.h"
#include "physics/constants.h"
#include "graphics/vertex.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

// Private functions
glm::dmat3 OrbitalObject::createR3matrix(double angle)
{
  return glm::dmat3(
      glm::dvec3(cos(angle), -sin(angle), 0),
      glm::dvec3(sin(angle), cos(angle), 0),
      glm::dvec3(0, 0, 1));
};

glm::dmat3 OrbitalObject::createR1matrix(double angle)
{
  return glm::dmat3(
      glm::dvec3(1, 0, 0),
      glm::dvec3(0, cos(angle), -sin(angle)),
      glm::dvec3(0, sin(angle), cos(angle)));
}

glm::dvec3 OrbitalObject::orbitalToInertial(double nu, const KeplerElements &keplerElements)
{
  double r = keplerElements.a * (1 - keplerElements.e * keplerElements.e) / (1 + keplerElements.e * cos(nu));
  glm::dvec3 orb(r * cos(nu), r * sin(nu), 0.0);
  return createR3matrix(keplerElements.Omega) * createR1matrix(keplerElements.i) * createR3matrix(keplerElements.omega) * orb;
}

void OrbitalObject::generateTrail(const KeplerElements &keplerElements)
{
  std::vector<glm::dvec3> trailVec;
  for (double nu = 0; nu < 2 * M_PI; nu += 0.01)
  {
    glm::dvec3 pos = this->orbitalToInertial(nu, keplerElements);
    pos += this->centralBody->getPosition();
    pos *= VISUAL_SCALE;
    trailVec.push_back(pos);
  }

  std::vector<Vertex> vertices;
  for (auto &p : trailVec)
  {
    Vertex v{};
    v.position = p;
    v.color = glm::vec3(1.f);
    v.texcoord = glm::vec2(0.0f);
    v.normal = glm::vec3(0.0f);

    vertices.push_back(v);
  }
  this->trail = std::make_unique<Mesh>(vertices.data(), vertices.size(), nullptr, 0, GL_LINE_STRIP);
}

// Constructor
OrbitalObject::OrbitalObject(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements) : Object(mu, radius), keplerElements(keplerElements)
{
  this->centralBody = centralBody;
  this->position = orbitalToInertial(0.0, keplerElements); // at periapsis
  this->position += centralBody->getPosition();
  this->orbit = std::make_unique<Orbit>(centralBody, keplerElements);
  if (this->orbit)
  {
    this->velocity = this->orbit->calculateOrbitalVelocity(centralBody, this);
    this->velocity += this->orbit->getCentralBody()->getVelocity();
    generateTrail(keplerElements);
  }
}

// Public functions
const Orbit *OrbitalObject::getOrbit() const
{
  return this->orbit.get();
}

void OrbitalObject::renderTrail(Shader *shader)
{
  shader->set1i(0, "useModelMatrix");
  shader->set1i(1, "isTexture");

  if (trail)
    trail->render(shader);
}

void OrbitalObject::move(double dt)
{
  this->velocity += 0.5 * this->acceleration * dt;
  this->keplerDrift(dt);
  this->velocity += 0.5 * this->acceleration * dt;

  this->renderPosition = this->position * VISUAL_SCALE;

  std::cout << "New position: " << renderPosition.x << ' ' << renderPosition.y << ' ' << renderPosition.z << std::endl;

  this->acceleration = glm::dvec3(0.f);
}

glm::dvec3 OrbitalObject::keplerDrift(double dt)
{
  double n = sqrt(this->centralBody->getMu() / pow(keplerElements.a, 3));
  double m = keplerElements.m + n * dt;

  m = fmod(m, 2 * M_PI);
  if (m < 0)
    m += 2 * M_PI;

  double E = m; // initial guess
  double delta;
  do
  {
    delta = (E - keplerElements.e * sin(E) - m) / (1 - keplerElements.e * cos(E));
    E = E - delta;
  } while (abs(delta) > EPS);

  glm::dvec3 pos(0.0);

  pos.x = keplerElements.a * (cos(E) - keplerElements.e);
  pos.y = -keplerElements.a * sqrt(1 - pow(keplerElements.e, 2)) * sin(E);

  glm::dvec3 v(0.0);
  double r = keplerElements.a * (1 - keplerElements.e * cos(E));

  v.x = -sqrt(this->centralBody->getMu() * keplerElements.a) / r * sin(E);
  v.y = sqrt(this->centralBody->getMu() * keplerElements.a * (1 - pow(keplerElements.e, 2))) / r * cos(E);

  glm::dmat3 R = createR3matrix(keplerElements.Omega) * createR1matrix(keplerElements.i) * createR3matrix(keplerElements.omega);

  this->velocity = R * v + this->centralBody->getVelocity();
  this->position = R * pos + this->centralBody->getPosition();
  keplerElements.m = m;
}