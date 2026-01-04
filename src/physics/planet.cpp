#include "physics/planet.h"
#include "physics/object.h"
#include "physics/constants.h"
#include "graphics/model.h"

#include <glm/glm.hpp>
#include <iostream>


// Constructor
Planet::Planet(glm::vec3 position, float mass, float radius, std::unique_ptr<Model> model, glm::vec3 velocity, Planet* centralBody, float orbitalPeriod) : Object(position, mass, radius, velocity)
{
  this->model = std::move(model);
  this->model->setPosition(this->position);
  this->centralBody = centralBody;
  this->orbitalPeriod = orbitalPeriod;
  if (this->centralBody)
  {
    this->velocity += this->calculateOrbitalSpeed(this->centralBody, this);
  }
}

// Public functions
void Planet::update(float dt)
{
  this->move(dt);
  this->model->setPosition(this->position);
}

void Planet::render(Shader *shader)
{
  if (model)
    model->render(shader);
}

Model* Planet::getModel() const
{
  return model.get();
}

// Static functions
glm::vec3 Planet::calculateOrbitalSpeed(const Planet* centralBody, const Planet* orbitBody)
{
  glm::vec3 normal(0.f, 0.05f, 1.f); // z-axis normal
  glm::vec3 velocity(0.f);
  
  glm::vec3 dp = centralBody->getPosition() - orbitBody->getPosition();
  // TO-DO Elliptical orbit
  glm::vec3 v_dir = glm::normalize(glm::cross(normal, dp));
  float speed = sqrt(G * centralBody->getMass()/ glm::length(dp));
  velocity = speed * v_dir;
  std::cout << "Orbital speed: " << velocity.x << ' ' << velocity.y << ' ' << velocity.z << std::endl;
  return velocity;
}
