#include "physics/atmosphere.h"

#include "debug/logger.h"

#include <iostream>
#include <fstream>

// Private functions
float Atmosphere::idx(double lon, double lat, double pres)
{
  if (!this->grid)
    Logger::logFatal("Atmosphere", "Grid is not defined for atmosphere");

  size_t idx = lon * this->grid->nlat * this->grid->npres + lat * this->grid->npres + pres;

  return idx;
}

void Atmosphere::readFloatFile(const std::string &path, std::vector<float> &data)
{
  std::ifstream file(path, std::ios::binary);
  if (!file)
    Logger::logFatal("Atmosphere", "Can not open file: " + path);

  size_t total = this->grid->nlon * this->grid->nlat * this->grid->npres;
  data.resize(total);

  file.read(reinterpret_cast<char *>(data.data()), total * sizeof(float));
}

void Atmosphere::initGrid(nlohmann::json &json)
{
  nlohmann::json grid;
  if (json.contains("dimensions"))
    grid = json["dimensions"];
  else
    Logger::logFatal("Atmosphere", "Metadata does not contain dimensions field");

  int nlon, nlat, npres = 0;
  if (grid.contains("longitude"))
    nlon = grid["longitude"];
  else
    Logger::logFatal("Atmosphere", "Grid does not contain longitude field");

  if (grid.contains("latitude"))
    nlat = grid["latitude"];
  else
    Logger::logFatal("Atmosphere", "Grid does not contain latitude field");

  if (grid.contains("pressure"))
    npres = grid["pressure"];
  else
    Logger::logFatal("Atmosphere", "Grid does not contain pressure field");

  this->grid = std::make_unique<Grid>(npres, nlat, nlon);
}

void Atmosphere::initFromMetadata(std::string &folderPath)
{
  std::string metaPath = folderPath + "/metadata.json";
  std::ifstream file(metaPath);

  if (!file.is_open())
  {
    Logger::logFatal("Atmosphere", "Can not open file: " + metaPath);
    return;
  }

  nlohmann::json json;
  file >> json;

  this->initGrid(json);

  if (json.contains("pressure_levels"))
    this->pressure = json["pressure_levels"].get<std::vector<float>>();
  else
    Logger::logFatal("Atmosphere", "Json does not contain pressure_levels field");

  if (json.contains("latitude_values"))
    this->latitude = json["latitude_values"].get<std::vector<float>>();
  else
    Logger::logFatal("Atmosphere", "Json does not contain latitude_values field");

  if (json.contains("longitude_values"))
    this->longitude = json["longitude_values"].get<std::vector<float>>();
  else
    Logger::logFatal("Atmosphere", "Json does not contain longitude_values field");
}

// Constructor
Atmosphere::Atmosphere(std::string &folderPath)
{
  this->initFromMetadata(folderPath);

  this->readFloatFile(folderPath + "/t.bin", this->temperature);
  this->readFloatFile(folderPath + "/r.bin", this->relativeHumidity);
  this->readFloatFile(folderPath + "/cc.bin", this->cloud);
  this->readFloatFile(folderPath + "/u.bin", this->u_wind);
  this->readFloatFile(folderPath + "/v.bin", this->v_wind);
  this->readFloatFile(folderPath + "/z.bin", this->geopotential);
}