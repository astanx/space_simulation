#pragma once

#include "resources/range.h"

#include "render/modelSource.h"
#include "physics/structs/radii.h"

#include "external/json.hpp"

#include <vector>
#include <string>

class Texture;
class Buffer;
class Shader;
class Planet;

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

  void calculateFlux(size_t index, glm::dvec3 flux, glm::uvec3 indices, std::vector<double> &data, std::vector<double> &mass) // vec3 - east, north, top
  {
    this->east[index] = flux.x * data[indices.x] / mass[indices.x];
    this->north[index] = flux.y * data[indices.y] / mass[indices.y];
    this->top[index] = flux.z * data[indices.z] / mass[indices.z];
  }

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
  Flux cloud_liquid_mass;
  Flux water_vapor_mass;
  Flux cloud_ice_mass;
  Flux cloud_rain_mass;
  Flux cloud_snow_mass;
  Flux ozone_mass;
  Flux energy;

  void forEach(std::function<void(Flux &)> &&func)
  {
    func(mass);
    func(cloud_liquid_mass);
    func(water_vapor_mass);
    func(cloud_ice_mass);
    func(cloud_rain_mass);
    func(cloud_snow_mass);
    func(ozone_mass);
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
  std::vector<float> relativeHumidity;
  std::vector<float> specificHumidity;
  // std::vector<float> divergence;
  std::vector<float> ozone_mass_mixing_ratio;
  // std::vector<float> vorticity;
  // std::vector<float> potential_vorticity;
  std::vector<float> cloud_cover;
  std::vector<float> cloud_ice_water_content;
  std::vector<float> cloud_liquid_water_content;
  std::vector<float> cloud_snow_water_content;
  std::vector<float> cloud_rain_water_content;

  std::vector<float> u_wind;
  std::vector<float> v_wind;
  std::vector<float> w_wind;
  std::vector<float> geopotential;
  std::vector<double> mass;
  std::vector<double> water_vapor_mass;
  std::vector<double> ozone_mass;
  std::vector<double> cloud_liquid_mass;
  std::vector<double> cloud_ice_mass;
  std::vector<double> cloud_rain_mass;
  std::vector<double> cloud_snow_mass;
  std::vector<double> thermal_energy;
  std::vector<double> density;
  std::vector<float> volume;

  std::vector<float> pressure;
  std::vector<float> latitude;
  std::vector<float> longitude;

  std::vector<float> faceAreaEW;
  std::vector<float> faceAreaNS;
  std::vector<float> faceAreaTB; // for each lon/lat

  std::unique_ptr<Texture> densityTexture;
  std::unique_ptr<Texture> temperatureTexture;
  std::unique_ptr<Texture> humidityTexture;
  std::unique_ptr<Texture> geopotentialTexture;
  std::unique_ptr<Texture> cloudTexture;
  std::unique_ptr<Texture> liquidContentTexture;
  std::unique_ptr<Texture> iceContentTexture;

  Grid grid;
  Radii radii;

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
  void initVectors(double g, Radii planetRadii);
  void initTextures();
  void initTexture(std::unique_ptr<Texture> &text);

  void updateTextures();
  void updateTexture(std::vector<float> &data, std::unique_ptr<Texture> &text);
  void updateTexture(std::vector<double> &data, std::unique_ptr<Texture> &text);

  void accumulateDelta(std::vector<float> &delta, float d, size_t idx, std::optional<size_t> nextIdx);
  double computeNetFlux(std::vector<double> &delta, size_t idx, std::optional<size_t> prevIdx);
  double computeNetFlux(Flux &delta, size_t index, size_t nlon, size_t nlat, size_t npres);

  void applyDelta();

  float calculateWaterVaporPressure(float q, float pressure);

public:
  Atmosphere(Planet *planet, std::string &folderPath, ThreadPool &threadPool);
  ~Atmosphere() = default;

  Radii getRadii() const { return this->radii; };

  void step(double dt, double g);

  void sendToShader(Shader &shader);

  void bindTextures();
  void unbindTextures();
};