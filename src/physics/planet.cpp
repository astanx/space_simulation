#include "physics/planet.h"
#include "physics/object.h"
#include "physics/constants.h"
#include "graphics/model.h"
#include "graphics/shader.h"
#include "graphics/vertex.h"

#include <glm/glm.hpp>
#include <iostream>

// Constructor
Planet::Planet(glm::vec3 position, float mass, float radius, std::unique_ptr<Model> model, glm::vec3 velocity, Planet *centralBody, float orbitalPeriod, glm::vec3 trailColor) : Object(position, mass, radius, velocity)
{
  this->model = std::move(model);
  this->model->setPosition(this->position);
  this->centralBody = centralBody;
  this->orbitalPeriod = orbitalPeriod;
  this->trailColor = trailColor;
  if (this->centralBody)
  {
    this->velocity = this->calculateOrbitalSpeed(this->centralBody, this);
  }
}

// Public functions
void Planet::update(float dt)
{
  this->move(dt);
  
  trail.push_back(this->position);
  if (trail.size() > maxTrailPoints)
    trail.pop_front();

  this->model->setPosition(this->position);
}

void Planet::render(Shader *shader)
{
  renderTrail(shader);

  if (model)
    model->render(shader);
}

Model *Planet::getModel() const
{
  return model.get();
}

float Planet::getOrbitalPeriod() const
{
  return this->orbitalPeriod;
}

void Planet::renderTrail(Shader *shader)
{
  if (trail.size() < 2)
    return;

  std::vector<Vertex> vertices;
  for (auto &p : trail)
  {
    Vertex v{};
    v.position = p;
    v.color = this->trailColor;
    v.texcoord = glm::vec2(0.0f);
    v.normal = glm::vec3(0.0f);

    vertices.push_back(v);
  }
  Mesh mesh(vertices.data(), vertices.size(), nullptr, 0, GL_LINE_STRIP);

  shader->set1i(0, "useModelMatrix");

  mesh.render(shader);
}

// Static functions
glm::vec3 Planet::calculateOrbitalSpeed(const Planet *centralBody, const Planet *orbitBody)
{
  glm::vec3 normal(0.f, 1.f, 0.f); // z-axis normal
  glm::vec3 velocity(0.f);

  glm::vec3 dp = centralBody->getPosition() - orbitBody->getPosition();

  float a;                                                                                         // semi-major axis
  a = cbrt(pow(orbitBody->getOrbitalPeriod(), 2) * G * centralBody->getMass() / 4 / pow(M_PI, 2)); // Kepler`s third law
  float r = glm::length(dp);
  float speed = sqrt(G * centralBody->getMass() * (2 / r - 1 / a));

  glm::vec3 v_dir = glm::normalize(glm::cross(normal, dp));
  velocity = speed * v_dir;
  std::cout << "Orbital speed: " << velocity.x << ' ' << velocity.y << ' ' << velocity.z << std::endl;
  return velocity;
}
