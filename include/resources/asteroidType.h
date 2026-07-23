#pragma once

#include "physics/structs/radii.h"

#include "graphics/model.h"

#include <memory>

struct AsteroidType
{
  const std::string &name;
  Model *model;
  Radii radii;
  double volume;

  AsteroidType(const std::string &name, Model *model, Radii radii, double volume) : name(name), model(model), radii(radii), volume(volume) {};
};