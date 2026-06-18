#pragma once

#include "external/json.hpp"

#include <vector>
#include <string>

struct Grid
{
  int npres;
  int nlat;
  int nlon;

  Grid(int npres, int nlat, int nlon) : npres(npres), nlat(nlat), nlon(nlon) {};
};

class Atmosphere
{
protected:
  std::vector<float> temperature;
  std::vector<float> relativeHumidity;
  std::vector<float> cloud;
  std::vector<float> u_wind;
  std::vector<float> v_wind;
  std::vector<float> geopotential;

  std::vector<float> pressure;
  std::vector<float> latitude;
  std::vector<float> longitude;

  std::unique_ptr<Grid> grid;

  float idx(double lon, double lat, double pres);
  void readFloatFile(const std::string &path, std::vector<float> &data);

  void initGrid(nlohmann::json &json);
  void initFromMetadata(std::string &folderPath);

public:
  Atmosphere(std::string &folderPath);
  ~Atmosphere() = default;
};