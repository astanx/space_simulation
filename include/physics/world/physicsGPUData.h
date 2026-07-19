#pragma once

#include "compute/clBuffer.h"

#include <glm/glm.hpp>

template <typename Real>
using Vec3 = glm::vec<3, Real>;

template <typename Real>
using Mat3 = glm::mat<3, 3, Real>;

template <typename Real>
using Quat = glm::qua<Real>;

template <typename Real>
struct DataGPU
{
  std::vector<Vec3<Real>> positions;
  std::vector<Vec3<Real>> velocities;
  std::vector<Real> mus;
  std::vector<Real> meanRadii;

  std::vector<Quat<Real>> orientations;
  std::vector<Vec3<Real>> angularVelocities;

  std::vector<Real> semiAxises;
  std::vector<Real> eccentricities;
  std::vector<Real> inclinations;
  std::vector<Real> longitude;
  std::vector<Real> periapsis;
  std::vector<Real> meanAnomaly;
  std::vector<Real> meanMotion;
  std::vector<int> centralBodyIndices;

  std::vector<Mat3<Real>> tensors;
  std::vector<int> loveIndices;
  std::vector<int> tidalFactorIndices;

  void resize(size_t n)
  {
    this->positions.resize(n);
    this->velocities.resize(n);
    this->mus.resize(n);
    this->meanRadii.resize(n);
    this->orientations.resize(n);
    this->angularVelocities.resize(n);
    this->semiAxises.resize(n);
    this->eccentricities.resize(n);
    this->inclinations.resize(n);
    this->longitude.resize(n);
    this->periapsis.resize(n);
    this->meanAnomaly.resize(n);
    this->meanMotion.resize(n);
    this->centralBodyIndices.resize(n);
    this->tensors.resize(n);
    this->loveIndices.resize(n);
    this->tidalFactorIndices.resize(n);
  }

  void combine(DataGPU &data)
  {
    this->positions.insert(this->positions.end(), std::make_move_iterator(data.positions.begin()), std::make_move_iterator(data.positions.end()));
    this->velocities.insert(this->velocities.end(), std::make_move_iterator(data.velocities.begin()), std::make_move_iterator(data.velocities.end()));
    this->mus.insert(this->mus.end(), std::make_move_iterator(data.mus.begin()), std::make_move_iterator(data.mus.end()));
    this->meanRadii.insert(this->meanRadii.end(), std::make_move_iterator(data.meanRadii.begin()), std::make_move_iterator(data.meanRadii.end()));
    this->orientations.insert(this->orientations.end(), std::make_move_iterator(data.orientations.begin()), std::make_move_iterator(data.orientations.end()));
    this->angularVelocities.insert(this->angularVelocities.end(), std::make_move_iterator(data.angularVelocities.begin()), std::make_move_iterator(data.angularVelocities.end()));
    this->semiAxises.insert(this->semiAxises.end(), std::make_move_iterator(data.semiAxises.begin()), std::make_move_iterator(data.semiAxises.end()));
    this->eccentricities.insert(this->eccentricities.end(), std::make_move_iterator(data.eccentricities.begin()), std::make_move_iterator(data.eccentricities.end()));
    this->inclinations.insert(this->inclinations.end(), std::make_move_iterator(data.inclinations.begin()), std::make_move_iterator(data.inclinations.end()));
    this->longitude.insert(this->longitude.end(), std::make_move_iterator(data.longitude.begin()), std::make_move_iterator(data.longitude.end()));
    this->periapsis.insert(this->periapsis.end(), std::make_move_iterator(data.periapsis.begin()), std::make_move_iterator(data.periapsis.end()));
    this->meanAnomaly.insert(this->meanAnomaly.end(), std::make_move_iterator(data.meanAnomaly.begin()), std::make_move_iterator(data.meanAnomaly.end()));
    this->meanMotion.insert(this->meanMotion.end(), std::make_move_iterator(data.meanMotion.begin()), std::make_move_iterator(data.meanMotion.end()));
    this->centralBodyIndices.insert(this->centralBodyIndices.end(), std::make_move_iterator(data.centralBodyIndices.begin()), std::make_move_iterator(data.centralBodyIndices.end()));
    this->tensors.insert(this->tensors.end(), std::make_move_iterator(data.tensors.begin()), std::make_move_iterator(data.tensors.end()));
    this->loveIndices.insert(this->loveIndices.end(), std::make_move_iterator(data.loveIndices.begin()), std::make_move_iterator(data.loveIndices.end()));
    this->tidalFactorIndices.insert(this->tidalFactorIndices.end(), std::make_move_iterator(data.tidalFactorIndices.begin()), std::make_move_iterator(data.tidalFactorIndices.end()));
  }
};

struct PhysicsGPUData
{  
  CLBuffer positionsBuffer;
  CLBuffer musBuffer;
  CLBuffer velocitiesBuffer;
  CLBuffer meanRadiiBuffer;
  CLBuffer polarRadiiBuffer;
  CLBuffer equatorianRadiiBuffer;

  CLBuffer orientationsBuffer;
  CLBuffer angularVelocitiesBuffer;

  CLBuffer tensorsBuffer;
  CLBuffer loveIndicesBuffer;
  CLBuffer tidalFactorIndicesBuffer;
  CLBuffer loveNumbersBuffer;
  CLBuffer tidalFactorsBuffer;

  CLBuffer semiAxisesBuffer;
  CLBuffer eccentricitiesBuffer;
  CLBuffer inclinationsBuffer;
  CLBuffer longitudeBuffer;
  CLBuffer periapsisBuffer;
  CLBuffer meanAnomalyBuffer;
  CLBuffer meanMotionBuffer;
  CLBuffer centralBodyIndicesBuffer;
};