#pragma once

#include "resources/gpuTypes.h"

#include <vector>

template <typename Real>
struct SharedDataGPU
{
  std::vector<Vec3<Real>> positions;
  std::vector<Quat<Real>> orientations;

  std::vector<Real> meanRadii;
  std::vector<Real> polarRadii;
  std::vector<Real> equatorianRadii;

  void resize(size_t n)
  {
    this->positions.resize(n);
    this->orientations.resize(n);
    this->meanRadii.resize(n);
    this->polarRadii.resize(n);
    this->equatorianRadii.resize(n);
  }

  void combine(SharedDataGPU &data)
  {
    this->positions.insert(this->positions.end(), std::make_move_iterator(data.positions.begin()), std::make_move_iterator(data.positions.end()));
    this->orientations.insert(this->orientations.end(), std::make_move_iterator(data.orientations.begin()), std::make_move_iterator(data.orientations.end()));
    this->meanRadii.insert(this->meanRadii.end(), std::make_move_iterator(data.meanRadii.begin()), std::make_move_iterator(data.meanRadii.end()));
    this->polarRadii.insert(this->polarRadii.end(), std::make_move_iterator(data.polarRadii.begin()), std::make_move_iterator(data.polarRadii.end()));
    this->equatorianRadii.insert(this->equatorianRadii.end(), std::make_move_iterator(data.equatorianRadii.begin()), std::make_move_iterator(data.equatorianRadii.end()));
  }
};