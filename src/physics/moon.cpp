#include "physics/moon.h"

#include "physics/constants.h"

#include "graphics/vertex.h"
#include "graphics/shader.h"

#include <iostream>

// Constructor
Moon::Moon(OrbitalObject *centralBody, double mu, double radius, const KeplerElements &keplerElements, const HapkeParameters &hapkeParameters) : OrbitalObject(centralBody, mu, radius, keplerElements, false), hapkeParameters(hapkeParameters)
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

void Moon::render(Shader &shader) const
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

void Moon::sendHapkeParametersToShader(Shader &shader) const
{
  shader.set1f(this->hapkeParameters.w, "hapkeParameters.w");
  shader.set1f(this->hapkeParameters.theta, "hapkeParameters.theta");
  shader.set1f(this->hapkeParameters.h, "hapkeParameters.h");
  shader.set1f(this->hapkeParameters.b0, "hapkeParameters.b0");
  shader.set1f(this->hapkeParameters.h_cb, "hapkeParameters.h_cb");
  shader.set1f(this->hapkeParameters.b0_cb, "hapkeParameters.b0_cb");
  shader.set1f(this->hapkeParameters.b, "hapkeParameters.b");
  shader.set1f(this->hapkeParameters.c, "hapkeParameters.c");
}