#pragma once

#include "resources/range.h"

#include "external/json.hpp"

#include <vector>
#include <string>

struct Grid
{
  int npres;
  int nlat;
  int nlon;

  size_t size;

  Grid(int npres = 0, int nlat = 0, int nlon = 0) : npres(npres), nlat(nlat), nlon(nlon)
  {
    size = npres * nlat * nlon;
  };

  void recalculateSize()
  {
    size = npres * nlat * nlon;
  }
};

struct Flux
{
  std::vector<double> east;
  std::vector<double> north;
  std::vector<double> top;

  void forEach(std::function<void(std::vector<double> &)> &&func)
  {
    func(east);
    func(north);
    func(top);
  }
};

struct ThreadScratch
{
  Flux mass;
  Flux cloud;
  Flux water;
  Flux energy;

  void forEach(std::function<void(Flux &)> &&func)
  {
    func(mass);
    func(cloud);
    func(water);
    func(energy);
  }
};

class ThreadPool;

class Atmosphere
{
protected:
  ThreadPool &threadPool;
  ThreadScratch scratch;
  std::vector<Range> threadRanges;

  std::vector<float> temperature;
  // std::vector<float> relativeHumidity;
  std::vector<float> specificHumidity;
  std::vector<float> cloud;
  std::vector<float> u_wind;
  std::vector<float> v_wind;
  std::vector<float> w_wind;
  std::vector<float> geopotential;
  std::vector<double> mass;
  std::vector<double> water_mass;
  std::vector<double> cloud_mass;
  std::vector<double> thermal_energy;
  std::vector<double> density;
  std::vector<float> volume;

  std::vector<float> pressure;
  std::vector<float> latitude;
  std::vector<float> longitude;

  std::vector<float> faceAreaEW;
  std::vector<float> faceAreaNS;
  std::vector<float> faceAreaTB; // for each lon/lat

  Grid grid;

  float g; // Planet gravitational acceleration

  size_t idx(size_t nlon, size_t nlat, size_t npres);
  std::optional<size_t> idx(std::optional<size_t> nlon, std::optional<size_t> nlat, std::optional<size_t> npres);
  size_t idx(size_t nlon, size_t nlat);
  size_t prevLon(size_t nlon);
  size_t nextLon(size_t nlon);
  std::optional<size_t> nextIdx(size_t idx, size_t size);
  std::optional<size_t> prevIdx(size_t idx);
  std::optional<float> vectorAt(std::vector<float> &data, std::optional<size_t> idx);

  void readFloatFile(const std::string &path, std::vector<float> &data);

  void initGrid(nlohmann::json &json);
  void initFromMetadata(std::string &folderPath);
  void initVectors();

  void accumulateDelta(std::vector<float> &delta, float d, size_t idx, std::optional<size_t> nextIdx);
  double computeNetFlux(std::vector<double> &delta, size_t idx, std::optional<size_t> prevIdx);
  double computeNetFlux(Flux &delta, size_t index, size_t nlon, size_t nlat, size_t npres);

  void applyDelta();

public:
  Atmosphere(std::string &folderPath, ThreadPool &threadPool);
  ~Atmosphere() = default;

  void step(float dt);
};