#include "physics/atmosphere.h"

#include "physics/atmosphereConstants.h"
#include "physics/constants.h"

#include "resources/threadPool.h"

#include "debug/logger.h"

#include <glm/glm.hpp>
#include <iostream>
#include <fstream>

// Private functions
size_t Atmosphere::idx(size_t nlon, size_t nlat, size_t npres)
{
  size_t idx = nlon * this->grid.nlat * this->grid.npres + nlat * this->grid.npres + npres;

  return idx;
}
std::optional<size_t> Atmosphere::idx(std::optional<size_t> nlon, std::optional<size_t> nlat, std::optional<size_t> npres)
{
  if (!nlon || !nlat || !npres)
    return std::nullopt;

  size_t idx = *nlon * this->grid.nlat * this->grid.npres + *nlat * this->grid.npres + *npres;

  return idx;
}

size_t Atmosphere::idx(size_t nlon, size_t nlat)
{
  size_t idx = nlon * this->grid.nlat + nlat;

  return idx;
}

size_t Atmosphere::nextLon(size_t nlon)
{
  return (nlon + 1) % this->longitude.size();
}

size_t Atmosphere::prevLon(size_t nlon)
{
  return (nlon == 0) ? this->longitude.size() - 1 : nlon - 1;
}

std::optional<size_t> Atmosphere::nextIdx(size_t idx, size_t size)
{
  if (idx + 1 < size)
    return idx + 1;
  return std::nullopt;
}

std::optional<size_t> Atmosphere::prevIdx(size_t idx)
{
  if (idx > 0)
    return idx - 1;
  return std::nullopt;
}

std::optional<float> Atmosphere::vectorAt(std::vector<float> &data, std::optional<size_t> idx)
{
  if (idx)
    return data[*idx];
  return std::nullopt;
}

void Atmosphere::readFloatFile(const std::string &path, std::vector<float> &data)
{
  std::ifstream file(path, std::ios::binary);
  if (!file)
    Logger::logFatal("Atmosphere", "Can not open file: " + path);

  size_t total = this->grid.nlon * this->grid.nlat * this->grid.npres;
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

  if (grid.contains("longitude"))
    this->grid.nlon = grid["longitude"];
  else
    Logger::logFatal("Atmosphere", "Grid does not contain longitude field");

  if (grid.contains("latitude"))
    this->grid.nlat = grid["latitude"];
  else
    Logger::logFatal("Atmosphere", "Grid does not contain latitude field");

  if (grid.contains("pressure"))
    this->grid.npres = grid["pressure"];
  else
    Logger::logFatal("Atmosphere", "Grid does not contain pressure field");

  this->grid.recalculateSize();
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

void Atmosphere::initVectors()
{
  double R = 6371000; // Planet mean radius change after
  float dlat = abs(this->latitude[1] - this->latitude[0]);
  float dlon = std::abs(longitude[1] - longitude[0]);
  double L_lat = R * M_PI * dlat / 180;

  size_t size = this->grid.size;

  this->mass.resize(size);
  this->cloud_mass.resize(size);
  this->water_mass.resize(size);
  this->density.resize(size);
  this->volume.resize(size);
  this->thermal_energy.resize(size);
  this->faceAreaEW.resize(size);
  this->faceAreaNS.resize(size);
  this->faceAreaTB.resize(this->grid.nlat * this->grid.nlon);

  for (Range &work : this->threadRanges)
  {
    this->threadPool.enqueue([this, &work, R, L_lat, dlon]()
                             {
      for (size_t nlon = work.begin; nlon < work.end; nlon++)
      {
        for (size_t nlat = 0; nlat < this->latitude.size(); nlat++)
        {
          float latRad = this->latitude[nlat] * M_PI / 180;
          double L_lon = R * cos(latRad) * dlon * M_PI / 180;
          this->faceAreaTB[idx(nlon, nlat)] = L_lon * L_lat; // top/bottom face area - for each lon/lat
          for (size_t npres = 0; npres < this->pressure.size(); npres++)
          {
            size_t index = idx(nlon, nlat, npres);
            float pres = this->pressure[npres];

            float h = this->geopotential[index] / this->g;

            float h_up = h;
            float h_down = h;
            if (auto next = this->nextIdx(npres, this->pressure.size()))
              h_up = this->geopotential[idx(nlon, nlat, *next)] / this->g;
            if (auto prev = this->prevIdx(npres))
              h_down = this->geopotential[idx(nlon, nlat, *prev)] / this->g;

            float L_up = abs(h_up - h);
            float L_down = abs(h - h_down);

            if (L_up == 0)
              L_up = L_down;
            if (L_down == 0)
              L_down = L_up;

            float L_vert = (L_up + L_down) / 2;

            float V = L_lon * L_lat * L_vert; // volume
            // this->volume[index] = std::max(V, float(EPS));
            this->volume[index] = V;

            float q = this->specificHumidity[index];
            float r_m = q / (1 - q);

            float e = r_m * pres / (0.622 + r_m);
            float pres_d = pres - e; // dry air pressure

            float T = this->temperature[index];
            double p = pres_d / (R_d * T) + e / (R_v * T); // density
            this->density[index] = p;
            // this->density[index] = std::max(p, float(EPS));

            float m = p * V; // mass
            this->mass[index] = m;

            float cc = this->cloud[index];

            this->water_mass[index] = q * m;           // water vapor mass
            this->cloud_mass[index] = cc * m;          // cloud water mass
            this->thermal_energy[index] = m * c_p * T; // thermal energy

            this->faceAreaEW[index] = L_lat * L_vert;          // east/west face area - for each cell
            this->faceAreaNS[index] = L_lon * L_vert;          // north/south face area - for each cell
          }
        }
} });
  }
  this->threadPool.wait();

  this->scratch.forEach([size](Flux &flux)
                        { flux.forEach([size](std::vector<double> &vec)
                                       { vec.resize(size); }); });

  std::cout << "INIT MASS: " << this->mass[27378] << std::endl;
  std::cout << "INIT DENSITY: " << this->density[27378] << std::endl;
  std::cout << "INIT TEMPERATURE: " << this->temperature[27378] << std::endl;
  std::cout << "INIT SPEED: " << this->w_wind[27378] << std::endl;
  std::cout << "INIT CLOUD: " << this->cloud[27378] << std::endl;
}

void Atmosphere::accumulateDelta(std::vector<float> &delta, float d, size_t idx, std::optional<size_t> nextIdx)
{
  delta[idx] -= d;
  if (nextIdx)
    delta[*nextIdx] += d;
}

double Atmosphere::computeNetFlux(std::vector<double> &delta, size_t idx, std::optional<size_t> prevIdx)
{
  double d = -delta[idx];
  if (prevIdx)
    d += delta[*prevIdx];

  return d;
}

double Atmosphere::computeNetFlux(Flux &delta, size_t index, size_t nlon, size_t nlat, size_t npres)
{
  return this->computeNetFlux(delta.east, index, idx(prevLon(nlon), nlat, npres)) +
         this->computeNetFlux(delta.north, index, idx(nlon, this->prevIdx(nlat), npres)) +
         this->computeNetFlux(delta.top, index, idx(nlon, nlat, this->prevIdx(npres)));
}

void Atmosphere::applyDelta()
{
  // std::cout << "FLUX: " << this->computeNetFlux(this->scratch.mass, 0, 0, 0, 0) << std::endl;

  for (Range &work : this->threadRanges)
  {
    this->threadPool.enqueue([this, &work]()
                             {
      for (size_t nlon = work.begin; nlon < work.end; nlon++)
      {
        for (size_t nlat = 0; nlat < this->latitude.size(); nlat++)
        {
          float A_tb = this->faceAreaTB[idx(nlon, nlat)];

          for (size_t npres = 0; npres < this->pressure.size(); npres++)
          {
            float pres = this->pressure[npres];
            size_t index = idx(nlon, nlat, npres);

            this->mass[index] += this->computeNetFlux(this->scratch.mass, index, nlon, nlat, npres);
            this->cloud_mass[index] += this->computeNetFlux(this->scratch.cloud, index, nlon, nlat, npres);
            this->water_mass[index] += this->computeNetFlux(this->scratch.water, index, nlon, nlat, npres);
            this->thermal_energy[index] += this->computeNetFlux(this->scratch.energy, index, nlon, nlat, npres);

            this->mass[index] = std::max(this->mass[index], 1e-200);
            double m = this->mass[index];

            this->specificHumidity[index] = this->water_mass[index] / float(m);
            this->cloud[index] = this->cloud_mass[index] / float(m);

            this->temperature[index] = this->thermal_energy[index] / (m * c_p);
            this->density[index] = m / this->volume[index];
          }
        }
      } });
  }
  this->threadPool.wait();

  // std::cout << "MASS 0: " << this->mass[27378] << std::endl;
  // std::cout << "DENSITY 0: " << this->density[27378] << std::endl;
  // std::cout << "ENERGY 0: " << this->thermal_energy[27378] << std::endl;
  // std::cout << "TEMPERATURE 0: " << this->temperature[27378] << std::endl;
  // std::cout << "WATER MASS 0: " << this->water_mass[27378] << std::endl;
  // std::cout << "CLOUD MASS 0: " << this->cloud_mass[27378] << std::endl;
  // std::cout << "HUMIDITY 0: " << this->specificHumidity[27378] << std::endl;
  // std::cout << "CLOUD 0: " << this->cloud[27378] << std::endl;
  // std::cout << std::endl;
}

// Constructor
Atmosphere::Atmosphere(std::string &folderPath, ThreadPool &threadPool) : threadPool(threadPool)
{
  this->g = 9.80665; // change

  this->initFromMetadata(folderPath);

  this->readFloatFile(folderPath + "/t.bin", this->temperature);
  // this->readFloatFile(folderPath + "/r.bin", this->relativeHumidity);
  this->readFloatFile(folderPath + "/q.bin", this->specificHumidity);
  this->readFloatFile(folderPath + "/cc.bin", this->cloud);
  this->readFloatFile(folderPath + "/u.bin", this->u_wind);
  this->readFloatFile(folderPath + "/v.bin", this->v_wind);
  this->readFloatFile(folderPath + "/w.bin", this->w_wind);
  this->readFloatFile(folderPath + "/z.bin", this->geopotential);

  this->threadPool.initRanges(this->threadRanges, this->longitude.size());

  this->initVectors();
}

// Public functions
void Atmosphere::step(float dt)
{
  this->scratch.forEach([](Flux &flux)
                        { flux.forEach([](std::vector<double> &vec)
                                       { std::fill(vec.begin(), vec.end(), 0.f); }); });

  for (size_t i = 0; i < this->threadRanges.size(); i++)
  {
    Range &work = this->threadRanges[i];
    this->threadPool.enqueue([this, &work, dt]()
                             {
  for (size_t nlon = work.begin; nlon < work.end; nlon++)
  {
    for (size_t nlat = 0; nlat < this->latitude.size(); nlat++)
    {
      float A_tb = this->faceAreaTB[idx(nlon, nlat)];

      for (size_t npres = 0; npres < this->pressure.size(); npres++)
      {
        float pres = this->pressure[npres];
        size_t index = idx(nlon, nlat, npres);

        // Face velocities
        float u_e = 0.f;
        float v_n = 0.f;
        float w_t = 0.f;

        float u = this->u_wind[index];
        float v = this->v_wind[index];
        float w = this->w_wind[index];

        size_t east = idx(this->nextLon(nlon), nlat, npres);
        std::optional<size_t> north = idx(nlon, this->nextIdx(nlat, this->latitude.size()), npres);
        std::optional<size_t> top = idx(nlon, nlat, this->nextIdx(npres, this->pressure.size()));

        u_e = (u + this->u_wind[east]) / 2;

        if (north)
          v_n = (v + this->v_wind[*north]) / 2;

        if (top)
          w_t = (w + this->w_wind[*top]) / 2;

        // Face densities
        float rho = this->density[index];
        float rho_e = rho;
        float rho_n = rho;

        rho_e = (rho + this->density[east]) / 2;

        if (north)
          rho_n = (rho + this->density[*north]) / 2;
        
        // Mass fluxes
        float f_e = rho_e * u_e * this->faceAreaEW[index];
        float f_n = rho_n * v_n * this->faceAreaNS[index];
        float f_t = -w_t * A_tb / g; // different because w_t - Pa/s

        // Mass transfered
        double m_e = f_e * dt;
        double m_n = f_n * dt;
        double m_t = f_t * dt;

        if (!std::isfinite(m_e)) m_e = 0;
        if (!std::isfinite(m_n)) m_n = 0;
        if (!std::isfinite(m_t)) m_t = 0;

        double m = this->mass[index];

        double c_w_e, c_c_e, c_e_e;
        double c_w_n, c_c_n, c_e_n;
        double c_w_t, c_c_t, c_e_t;

        if (m_e < 0)
        {
          double m = this->mass[east];

          c_w_e = this->water_mass[east] / m;
          c_c_e = this->cloud_mass[east] / m;
          c_e_e = this->thermal_energy[east] / m;
        }
        else
        {
          c_w_e = this->water_mass[index] / m;
          c_c_e = this->cloud_mass[index] / m;
          c_e_e = this->thermal_energy[index] / m;
        }

        if (m_n < 0 && north)
        {
          double m = this->mass[*north];

          c_w_n = this->water_mass[*north] / m;
          c_c_n = this->cloud_mass[*north] / m;
          c_e_n = this->thermal_energy[*north] / m;
        }
        else
        {
          c_w_n = this->water_mass[index] / m;
          c_c_n = this->cloud_mass[index] / m;
          c_e_n = this->thermal_energy[index] / m;
        }

        if (m_t < 0 && top)
        {
          double m = this->mass[*top];

          c_w_t = this->water_mass[*top] / m;
          c_c_t = this->cloud_mass[*top] / m;
          c_e_t = this->thermal_energy[*top] / m; 
        }
        else
        {
          c_w_t = this->water_mass[index] / m;
          c_c_t = this->cloud_mass[index] / m;
          c_e_t = this->thermal_energy[index] / m;
        }


        this->scratch.mass.east[index] = m_e;
        this->scratch.mass.north[index] = m_n;
        this->scratch.mass.top[index] = m_t;

        this->scratch.water.east[index] = m_e * c_w_e;
        this->scratch.water.north[index] = m_n * c_w_n;
        this->scratch.water.top[index] = m_t * c_w_t;

        this->scratch.cloud.east[index] = m_e * c_c_e;
        this->scratch.cloud.north[index] = m_n * c_c_n;
        this->scratch.cloud.top[index] = m_t * c_c_t;

        this->scratch.energy.east[index] = m_e * c_e_e;
        this->scratch.energy.north[index] = m_n * c_e_n;
        this->scratch.energy.top[index] = m_t * c_e_t;
      }
    }
  } });
  }

  this->threadPool.wait();

  std::vector<double> limiter;
  limiter.resize(this->grid.size);

  for (size_t i = 0; i < this->threadRanges.size(); i++)
  {
    Range &work = this->threadRanges[i];
    this->threadPool.enqueue([this, &work, &limiter]()
                             {
  for (size_t nlon = work.begin; nlon < work.end; nlon++)
  {
    for (size_t nlat = 0; nlat < this->latitude.size(); nlat++)
    {
      for (size_t npres = 0; npres < this->pressure.size(); npres++)
      {
        size_t index = idx(nlon, nlat, npres);
        size_t west = idx(prevLon(nlon), nlat, npres);
        std::optional<size_t> south = idx(nlon, prevIdx(nlat), npres);
        std::optional<size_t> bottom = idx(nlon, nlat, prevIdx(npres));

        double outgoing = 0.0;

        outgoing += std::max(0.0, scratch.mass.east[index]);
        outgoing += std::max(0.0, scratch.mass.north[index]);
        outgoing += std::max(0.0, scratch.mass.top[index]);

        outgoing += std::max(0.0, -scratch.mass.east[west]);
        if (south) outgoing += std::max(0.0, -scratch.mass.north[*south]);
        if (bottom) outgoing += std::max(0.0, -scratch.mass.top[*bottom]);

        if (outgoing <= 0.0 || !std::isfinite(outgoing))
          limiter[index] = 1.0;
        else
          limiter[index] = std::min(1.0, this->mass[index] / outgoing);
      }
    }
  } });
  }

  this->threadPool.wait();

  for (size_t i = 0; i < this->threadRanges.size(); i++)
  {
    Range &work = this->threadRanges[i];
    this->threadPool.enqueue([this, &work, &limiter]()
                             {
  for (size_t nlon = work.begin; nlon < work.end; nlon++)
  {
    for (size_t nlat = 0; nlat < this->latitude.size(); nlat++)
    {
      for (size_t npres = 0; npres < this->pressure.size(); npres++)
      {
        size_t index = idx(nlon, nlat, npres);
        size_t east = idx(this->nextLon(nlon), nlat, npres);
        std::optional<size_t> north = idx(nlon, this->nextIdx(nlat, this->latitude.size()), npres);
        std::optional<size_t> top = idx(nlon, nlat, this->nextIdx(npres, this->pressure.size()));

        this->scratch.forEach([&limiter, index, east, north, top](Flux& flux)
      {
        flux.east[index] *= std::min(
          limiter[index],
          limiter[east]
        );
        
        flux.north[index] *= std::min(
          limiter[index],
          north ? limiter[*north] : 1.f
        );

        flux.top[index] *= std::min(
          limiter[index],
          top ? limiter[*top] : 1.f
        );
      });
      }
    }
  } });
  }

  this->threadPool.wait();


  // double total = 0.0;
  // for (size_t i = 0; i < mass.size(); i++)
  //   total += mass[i];
  // std::cout << "TOTAL: " << total << std::endl;

  this->applyDelta();
}