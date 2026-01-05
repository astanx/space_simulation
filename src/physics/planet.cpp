#include "physics/planet.h"
#include "physics/object.h"
#include "physics/constants.h"
#include "physics/orbit.h"
#include "graphics/model.h"
#include "graphics/shader.h"
#include "graphics/vertex.h"

#include <glm/glm.hpp>
#include <iostream>

// Constructor
Planet::Planet(glm::dvec3 position, double mass, double radius, std::unique_ptr<Model> model,
               glm::dvec3 velocity, std::unique_ptr<Orbit> orbit) : Object(position, mass, radius, velocity)
{
  this->renderPosition = position * VISUAL_SCALE;
  this->model = std::move(model);
  this->model->setPosition(this->renderPosition);
  this->orbit = std::move(orbit);
  if (this->orbit)
  {
    this->velocity = this->orbit->calculateOrbitalVelocity(this->orbit->getCentralBody(), this);
    this->velocity += this->orbit->getCentralBody()->getVelocity();
  }
}

// Public functions
void Planet::update(double dt)
{
  this->move(dt);

  if (this->orbit)
    this->orbit->updateTrail(this->renderPosition);

  this->model->setPosition(this->renderPosition);
}

void Planet::render(Shader *shader)
{
  if (this->orbit)
    this->orbit->renderTrail(shader);

  if (model)
    model->render(shader);
}

Model *Planet::getModel() const
{
  return this->model.get();
}

Orbit *Planet::getOrbit() const
{
  return this->orbit.get();
}
