#include <physics/star.h>
#include <physics/constants.h>
#include <graphics/model.h>

// Constructor
Star::Star(double mu, double radius, glm::dvec3 position, glm::dvec3 velocity) : Object(mu / G, radius, position, velocity)
{
  this->mu = mu;
}

// Public functions
void Star::update(double dt)
{
  // this->move(dt);
  if (this->model)
    this->model->setPosition(this->renderPosition);
}

void Star::render(Shader &shader)
{
  if (model)
    model->render(shader);
}

void Star::addModel(std::unique_ptr<Model> model)
{
  this->model = std::move(model);
  this->model->setPosition(this->renderPosition);
};
