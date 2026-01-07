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
  this->move(dt);
  if (this->model)
    this->model->setPosition(this->renderPosition);
}

void Planet::render(Shader *shader)
{
  if (this->orbit)
    this->renderTrail(shader);

  if (model)
    model->render(shader);
};

void Planet::addModel(std::unique_ptr<Model> model)
{
  this->model = std::move(model);
  this->model->setPosition(this->position * VISUAL_SCALE);
};
