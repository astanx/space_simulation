#pragma once

#include "physics/structs/radii.h"

#include "graphics/model.h"

#include <memory>

struct AsteroidType
{
  const std::string &name;
  std::unique_ptr<Model> model;
  Radii radii;
  double volume;

  AsteroidType(const std::string &name, std::unique_ptr<Model> model, Radii radii, double volume) : name(name), model(std::move(model)), radii(radii), volume(volume) {};
};