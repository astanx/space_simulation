#include "physics/moon.h"
#include "physics/constants.h"

// Constructor
Moon::Moon(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements) : OrbitalObject(centralBody, mu, radius, keplerElements)
{
}

// Public functions
void Moon::update(double dt)
{
  this->move(dt);
  this->model->setPosition(this->renderPosition);
}

void Moon::render(Shader *shader)
{
  if (this->orbit)
    this->renderTrail(shader);

  if (model)
    model->render(shader);
}

void Moon::addModel(std::unique_ptr<Model> model)
{
  this->model = std::move(model);
  this->model->setPosition(this->position * VISUAL_SCALE);
};
