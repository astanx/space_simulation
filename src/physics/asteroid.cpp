#include "physics/asteroid.h"
#include "physics/constants.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Constructor
Asteroid::Asteroid(Object *centralBody, double mu, double radius, const KeplerElements &elements) : OrbitalObject(centralBody, mu, radius, elements)
{
}

// Public functions
void Asteroid::update(double dt)
{
  // this->move(dt);
  // if (this->model)
  //   this->model->setPosition(this->renderPosition);
}

void Asteroid::render(Shader &shader)
{
  if (model)
    model->render(shader);
};

void Asteroid::renderInstanced()
{
  if (model)
    model->renderInstanced();
};

void Asteroid::addModel(std::unique_ptr<Model> model)
{
  this->model = std::move(model);
  this->model->setPosition(this->renderPosition);
};

glm::mat4 Asteroid::getModelMatrix()
{
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(this->renderPosition));
  return model;
};
