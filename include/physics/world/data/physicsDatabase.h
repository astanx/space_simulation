#pragma once

#include "resources/gpuTypes.h"

#include <vector>

template <typename Real>
struct PhysicsDatabase
{
  std::vector<Vec3<Real>> velocities;
  std::vector<Real> mus;

  std::vector<Vec3<Real>> angularVelocities;

  std::vector<Real> semiAxises;
  std::vector<Real> eccentricities;
  std::vector<Real> inclinations;
  std::vector<Real> longitude;
  std::vector<Real> periapsis;
  std::vector<Real> meanAnomaly;
  std::vector<Real> meanMotion;
  std::vector<int> centralBodyIndices;

  std::vector<Mat3<Real>> quadrupoleTensors;
  std::vector<Mat3<Real>> inertiaTensors;
  std::vector<int> loveIndices;
  std::vector<int> tidalFactorIndices;
  std::vector<Real> loveNumbers;
  std::vector<Real> tidalFactors;

  void resize(size_t n)
  {
    this->velocities.resize(n);
    this->mus.resize(n);
    this->angularVelocities.resize(n);
    this->semiAxises.resize(n);
    this->eccentricities.resize(n);
    this->inclinations.resize(n);
    this->longitude.resize(n);
    this->periapsis.resize(n);
    this->meanAnomaly.resize(n);
    this->meanMotion.resize(n);
    this->centralBodyIndices.resize(n);
    this->quadrupoleTensors.resize(n);
    this->inertiaTensors.resize(n);
    this->loveIndices.resize(n);
    this->tidalFactorIndices.resize(n);
  }

  void combine(PhysicsDatabase &data)
  {
    this->velocities.insert(this->velocities.end(), std::make_move_iterator(data.velocities.begin()), std::make_move_iterator(data.velocities.end()));
    this->mus.insert(this->mus.end(), std::make_move_iterator(data.mus.begin()), std::make_move_iterator(data.mus.end()));
    this->angularVelocities.insert(this->angularVelocities.end(), std::make_move_iterator(data.angularVelocities.begin()), std::make_move_iterator(data.angularVelocities.end()));
    this->semiAxises.insert(this->semiAxises.end(), std::make_move_iterator(data.semiAxises.begin()), std::make_move_iterator(data.semiAxises.end()));
    this->eccentricities.insert(this->eccentricities.end(), std::make_move_iterator(data.eccentricities.begin()), std::make_move_iterator(data.eccentricities.end()));
    this->inclinations.insert(this->inclinations.end(), std::make_move_iterator(data.inclinations.begin()), std::make_move_iterator(data.inclinations.end()));
    this->longitude.insert(this->longitude.end(), std::make_move_iterator(data.longitude.begin()), std::make_move_iterator(data.longitude.end()));
    this->periapsis.insert(this->periapsis.end(), std::make_move_iterator(data.periapsis.begin()), std::make_move_iterator(data.periapsis.end()));
    this->meanAnomaly.insert(this->meanAnomaly.end(), std::make_move_iterator(data.meanAnomaly.begin()), std::make_move_iterator(data.meanAnomaly.end()));
    this->meanMotion.insert(this->meanMotion.end(), std::make_move_iterator(data.meanMotion.begin()), std::make_move_iterator(data.meanMotion.end()));
    this->centralBodyIndices.insert(this->centralBodyIndices.end(), std::make_move_iterator(data.centralBodyIndices.begin()), std::make_move_iterator(data.centralBodyIndices.end()));
    this->quadrupoleTensors.insert(this->quadrupoleTensors.end(), std::make_move_iterator(data.quadrupoleTensors.begin()), std::make_move_iterator(data.quadrupoleTensors.end()));
    this->inertiaTensors.insert(this->inertiaTensors.end(), std::make_move_iterator(data.inertiaTensors.begin()), std::make_move_iterator(data.inertiaTensors.end()));
    this->loveIndices.insert(this->loveIndices.end(), std::make_move_iterator(data.loveIndices.begin()), std::make_move_iterator(data.loveIndices.end()));
    this->tidalFactorIndices.insert(this->tidalFactorIndices.end(), std::make_move_iterator(data.tidalFactorIndices.begin()), std::make_move_iterator(data.tidalFactorIndices.end()));
  }
};