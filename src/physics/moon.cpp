#include "physics/moon.h"

#include "physics/constants.h"

#include "graphics/vertex.h"

#include <iostream>

// Constructor
Moon::Moon(OrbitalObject *centralBody, double mu, double radius, const KeplerElements &keplerElements) : OrbitalObject(centralBody, mu, radius, keplerElements, false)
{
  if (this->useTrail)
    this->generateTrail();
}

// Public functions
void Moon::update(double dt)
{
  // this->move(dt);

  if (this->model)
    this->model->setPosition(this->renderPosition);
}

void Moon::render(Shader &shader)
{
  if (model)
    model->render(shader);
}

void Moon::addModel(std::unique_ptr<Model> model)
{
  this->model = std::move(model);

  this->model->setPosition(this->renderPosition);
};

std::unique_ptr<Trail> Moon::generateTrail()
{
  std::vector<glm::dvec3> trailVec;
  for (double nu = 0; nu < 2 * M_PI; nu += 0.01)
  {
    glm::dvec3 pos = this->orbitalToInertial(nu);
    pos += this->orbit->getCentralBody()->getPosition();
    pos = this->realToVisualPos(pos);

    trailVec.push_back(pos);
  }

  return std::make_unique<Trail>(trailVec);
}