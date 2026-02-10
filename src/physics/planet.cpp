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
Planet::Planet(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements) : OrbitalObject(centralBody, mu, radius, keplerElements)
{
}

// Public functions
void Planet::update(double dt)
{
  // this->move(dt);

  for (std::unique_ptr<Moon> &moon : this->moons)
  {
    moon->update(dt);
  }
  if (this->model)
    this->model->setPosition(this->renderPosition);
}

void Planet::render(Shader &shader)
{
  for (std::unique_ptr<Moon> &moon : this->moons)
  {
    moon->render(shader);
  }

  if (this->model)
    this->model->render(shader);
};

void Planet::addModel(std::unique_ptr<Model> model)
{
  this->model = std::move(model);
  this->model->setPosition(this->renderPosition);
};

void Planet::addMoon(std::unique_ptr<Moon> moon)
{
  this->moons.push_back(std::move(moon));
}

void Planet::drift(double dt)
{
  this->keplerDrift(dt);

  for (std::unique_ptr<Moon> &moon : this->moons)
    moon->drift(dt);
}

void Planet::halfKick(const std::vector<Object *> &bodies, double dt)
{
  this->kick(bodies, dt * 0.5);

  for (std::unique_ptr<Moon> &moon : this->moons)
    moon->halfKick(bodies, dt);
}