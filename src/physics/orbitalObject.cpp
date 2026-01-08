#include "physics/orbitalObject.h"
#include "physics/constants.h"
#include "graphics/vertex.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

// Private functions
glm::dvec3 OrbitalObject::realToVisualPos(glm::dvec3 pos)
{
  return glm::dvec3(
             pos.x,
             pos.z, // Z → Y
             pos.y  // Y → -Z
             ) *
         VISUAL_SCALE;
}
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

glm::dvec3 OrbitalObject::orbitalToInertial(double nu)
{
  KeplerElements keplerElements = this->orbit->getKeplerElements();
  double r = keplerElements.a * (1 - keplerElements.e * keplerElements.e) / (1 + keplerElements.e * cos(nu));
  glm::dvec3 orb(r * cos(nu), r * sin(nu), 0.0);
  return createR3matrix(keplerElements.Omega) * createR1matrix(keplerElements.i) * createR3matrix(keplerElements.omega) * orb;
}

void OrbitalObject::generateTrail()
{
  std::vector<glm::dvec3> trailVec;
  for (double nu = 0; nu < 2 * M_PI; nu += 0.01)
  {
    glm::dvec3 pos = this->orbitalToInertial(nu);
    pos += this->orbit->getCentralBody()->getPosition();
    pos = this->realToVisualPos(pos);

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
  this->trail = std::make_unique<Mesh>(vertices.data(), vertices.size(), nullptr, 0, GL_LINE_LOOP);
}

// Constructor
OrbitalObject::OrbitalObject(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements) : Object(mu, radius)
{
  this->orbit = std::make_unique<Orbit>(centralBody, keplerElements);
  this->position = orbitalToInertial(0.0); // at periapsis
  this->position += centralBody->getPosition();
  if (this->orbit)
  {
    this->velocity = this->orbit->calculateOrbitalVelocity(centralBody, this);
    this->velocity += this->orbit->getCentralBody()->getVelocity();
    if (this->useTrail)
      generateTrail();
  }
}

// Public functions
const Orbit *OrbitalObject::getOrbit() const
{
  return this->orbit.get();
}

void OrbitalObject::renderTrail(Shader *shader)
{
  if (!trail || !this->useTrail)
    return;

  shader->set1i(0, "useModelMatrix");
  shader->set1i(1, "isTexture");

  trail->render(shader);
}

void OrbitalObject::move(double dt)
{
  this->velocity += 0.5 * this->acceleration * dt;
  this->keplerDrift(dt);
  this->velocity += 0.5 * this->acceleration * dt;

  this->renderPosition = this->realToVisualPos(this->position);

  std::cout << "New position: " << renderPosition.x << ' ' << renderPosition.y << ' ' << renderPosition.z << std::endl;

  this->acceleration = glm::dvec3(0.f);
}

void OrbitalObject::keplerDrift(double dt)
{
  KeplerElements keplerElements = this->orbit->getKeplerElements();
  Object *centralBody = this->orbit->getCentralBody();
  double n = sqrt(centralBody->getMu() / pow(keplerElements.a, 3));
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
  pos.y = keplerElements.a * sqrt(1 - pow(keplerElements.e, 2)) * sin(E);

  glm::dvec3 v(0.0);
  double r = keplerElements.a * (1 - keplerElements.e * cos(E));

  v.x = -sqrt(centralBody->getMu() * keplerElements.a) / r * sin(E);
  v.y = sqrt(centralBody->getMu() * keplerElements.a * (1 - pow(keplerElements.e, 2))) / r * cos(E);

  glm::dmat3 R = createR3matrix(keplerElements.Omega) * createR1matrix(keplerElements.i) * createR3matrix(keplerElements.omega);

  this->velocity = R * v + centralBody->getVelocity();
  this->position = R * pos + centralBody->getPosition();
  keplerElements.m = m;
  this->orbit->updateKeplerElements(keplerElements);
}