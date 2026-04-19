#pragma once

#include "physics/object.h"
#include "physics/wisdomHolman.h"

#include "render/modelSource.h"

class Model;

class Star : public Object, public ModelSource, public WisdomHolman
{
protected:
  double luminosity;
public:
  Star(double mu, double radius, double luminosity, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));
  ~Star() = default;

  void drift(double dt) override;
  void halfKick(const std::vector<Object *> &bodies, double dt) override;

  double getLuminosity() const { return this->luminosity; }
};