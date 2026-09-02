#pragma once

#include "resources/gpuTypes.h"
#include "physics/world/total.h"

#include <vector>

template <typename Real>
struct SharedDatabase
{
  std::vector<Vec3<Real>> positions;
  std::vector<Quat<Real>> orientations;

  std::vector<Real> luminosities;

  std::vector<Real> meanRadii;
  std::vector<Real> polarRadii;
  std::vector<Real> equatorianRadii;

  Total total;

  void resize(size_t n)
  {
    this->positions.resize(n);
    this->luminosities.resize(n);
    this->orientations.resize(n);
    this->meanRadii.resize(n);
    this->polarRadii.resize(n);
    this->equatorianRadii.resize(n);
  }

  void combine(SharedDatabase &data)
  {
    this->positions.insert(this->positions.end(), std::make_move_iterator(data.positions.begin()), std::make_move_iterator(data.positions.end()));
    this->luminosities.insert(this->luminosities.end(), std::make_move_iterator(data.luminosities.begin()), std::make_move_iterator(data.luminosities.end()));
    this->orientations.insert(this->orientations.end(), std::make_move_iterator(data.orientations.begin()), std::make_move_iterator(data.orientations.end()));
    this->meanRadii.insert(this->meanRadii.end(), std::make_move_iterator(data.meanRadii.begin()), std::make_move_iterator(data.meanRadii.end()));
    this->polarRadii.insert(this->polarRadii.end(), std::make_move_iterator(data.polarRadii.begin()), std::make_move_iterator(data.polarRadii.end()));
    this->equatorianRadii.insert(this->equatorianRadii.end(), std::make_move_iterator(data.equatorianRadii.begin()), std::make_move_iterator(data.equatorianRadii.end()));
  }
};