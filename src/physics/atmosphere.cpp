#include "physics/atmosphere.h"

#include "physics/atmosphereConstants.h"
#include "physics/constants.h"

#include "resources/threadPool.h"

#include "graphics/texture.h"
#include "graphics/buffers/buffer.h"

#include "graphics/state/scopedTexture.h"
#include "graphics/state/scopedBuffer.h"

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

float Atmosphere::calculateWaterVaporPressure(float q, float pressure)
{
  float r_m = q / (1 - q);

  float e = r_m * pressure / (0.622 + r_m);

  return e;
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

void Atmosphere::initVectors(double g, double R)
{
  float dlat = abs(this->latitude[1] - this->latitude[0]);
  float dlon = std::abs(longitude[1] - longitude[0]);
  double L_lat = R * M_PI * dlat / 180;

  size_t size = this->grid.size;

  this->mass.resize(size);
  this->cloud_liquid_mass.resize(size);
  this->water_vapor_mass.resize(size);
  this->ozone_mass.resize(size);
  this->cloud_snow_mass.resize(size);
  this->cloud_rain_mass.resize(size);
  this->cloud_ice_mass.resize(size);
  this->density.resize(size);
  this->volume.resize(size);
  this->thermal_energy.resize(size);
  this->faceAreaEW.resize(size);
  this->faceAreaNS.resize(size);
  this->faceAreaTB.resize(this->grid.nlat * this->grid.nlon);

  for (Range &work : this->threadRanges)
  {
    this->threadPool.enqueue([this, &work, R, L_lat, dlon, g]()
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

            float h = this->geopotential[index] / g;

            float h_up = h;
            float h_down = h;
            if (auto next = this->nextIdx(npres, this->pressure.size()))
              h_up = this->geopotential[idx(nlon, nlat, *next)] / g;
            if (auto prev = this->prevIdx(npres))
              h_down = this->geopotential[idx(nlon, nlat, *prev)] / g;

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
            float e = this->calculateWaterVaporPressure(q, pres);

            float pres_d = pres - e; // dry air pressure

            float T = this->temperature[index];
            double p = pres_d / (R_d * T) + e / (R_v * T); // density
            this->density[index] = p;
            // this->density[index] = std::max(p, float(EPS));

            float m = p * V; // mass
            this->mass[index] = m;

            this->water_vapor_mass[index] = q * m;  // water vapor mass
            this->ozone_mass[index] = this->ozone_mass_mixing_ratio[index] * m;
            this->cloud_ice_mass[index] = this->cloud_ice_water_content[index] * m;
            this->cloud_liquid_mass[index] = this->cloud_liquid_water_content[index] * m;
            this->cloud_snow_mass[index] = this->cloud_snow_water_content[index] * m;
            this->cloud_rain_mass[index] = this->cloud_rain_water_content[index] * m;
            this->thermal_energy[index] = m * c_p * T; // thermal energy

            this->relativeHumidity[index] /= 100; // convert from percent

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

  std::cout << "INIT MASS: " << this->mass[1000] << std::endl;
  std::cout << "INIT DENSITY: " << this->density[1000] << std::endl;
  std::cout << "INIT TEMPERATURE: " << this->temperature[1000] << std::endl;
  std::cout << "INIT SPEED: " << this->w_wind[1000] << std::endl;
  std::cout << "INIT CLOUD COVER: " << this->cloud_cover[1000] << std::endl;
  std::cout << "INIT RELATIVE HUMIDITY: " << this->relativeHumidity[1000] << std::endl;
}

void Atmosphere::initTexture(std::unique_ptr<Texture> &text)
{
  text = std::make_unique<Texture>(GL_TEXTURE_3D);

  ScopedTexture t(*text);

  GL_CALL(glTexImage3D(
      GL_TEXTURE_3D,
      0,
      GL_R16F,
      this->grid.nlon,
      this->grid.nlat,
      this->grid.npres,
      0,
      GL_RED,
      GL_FLOAT,
      nullptr));
}

void Atmosphere::initTextures()
{
  this->initTexture(this->densityTexture);
  this->initTexture(this->cloudTexture);
  this->initTexture(this->temperatureTexture);
  this->initTexture(this->humidityTexture);
  this->initTexture(this->geopotentialTexture);
}

void Atmosphere::updateTexture(std::vector<float> &data, std::unique_ptr<Texture> &text)
{
  ScopedTexture t(*text);
  glTexSubImage3D(
      GL_TEXTURE_3D,
      0,
      0, 0, 0,
      this->grid.nlon,
      this->grid.nlat,
      this->grid.npres,
      GL_RED,
      GL_FLOAT,
      data.data());
}

void Atmosphere::updateTexture(std::vector<double> &data, std::unique_ptr<Texture> &text)
{
  ScopedTexture t(*text);
  glTexSubImage3D(
      GL_TEXTURE_3D,
      0,
      0, 0, 0,
      grid.nlon,
      grid.nlat,
      grid.npres,
      GL_RED,
      GL_FLOAT,
      data.data());
}

void Atmosphere::updateTextures()
{
  this->updateTexture(this->density, this->densityTexture);
  this->updateTexture(this->specificHumidity, this->humidityTexture);
  this->updateTexture(this->temperature, this->temperatureTexture);
  this->updateTexture(this->cloud_cover, this->cloudTexture);
  this->updateTexture(this->geopotential, this->geopotentialTexture);
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
            this->cloud_ice_mass[index] += this->computeNetFlux(this->scratch.cloud_ice_mass, index, nlon, nlat, npres);
            this->cloud_liquid_mass[index] += this->computeNetFlux(this->scratch.cloud_liquid_mass, index, nlon, nlat, npres);
            this->cloud_snow_mass[index] += this->computeNetFlux(this->scratch.cloud_snow_mass, index, nlon, nlat, npres);
            this->cloud_rain_mass[index] += this->computeNetFlux(this->scratch.cloud_rain_mass, index, nlon, nlat, npres);
            this->water_vapor_mass[index] += this->computeNetFlux(this->scratch.water_vapor_mass, index, nlon, nlat, npres);
            this->ozone_mass[index] += this->computeNetFlux(this->scratch.ozone_mass, index, nlon, nlat, npres);
            this->thermal_energy[index] += this->computeNetFlux(this->scratch.energy, index, nlon, nlat, npres);

            this->mass[index] = std::max(this->mass[index], 1e-200);
            double m = this->mass[index];

            this->specificHumidity[index] = std::clamp(this->water_vapor_mass[index] / float(m), 0.0, 1.0);
            this->cloud_ice_water_content[index] = std::clamp(this->cloud_ice_mass[index] / float(m), 0.0, 1.0);
            this->cloud_liquid_water_content[index] = std::clamp(this->cloud_liquid_mass[index] / float(m), 0.0, 1.0);
            this->cloud_rain_water_content[index] = std::clamp(this->cloud_rain_mass[index] / float(m), 0.0, 1.0);
            this->cloud_snow_water_content[index] = std::clamp(this->cloud_snow_mass[index] / float(m), 0.0, 1.0);
            this->ozone_mass_mixing_ratio[index] = std::clamp(this->ozone_mass[index] / float(m), 0.0, 1.0);

            if (!isfinite(this->specificHumidity[index])) this->specificHumidity[index] = 0.0;
            if (!isfinite(this->cloud_ice_water_content[index])) this->cloud_ice_water_content[index] = 0.0;
            if (!isfinite(this->cloud_liquid_water_content[index])) this->cloud_liquid_water_content[index] = 0.0;
            if (!isfinite(this->cloud_rain_water_content[index])) this->cloud_rain_water_content[index] = 0.0;
            if (!isfinite(this->cloud_snow_water_content[index])) this->cloud_snow_water_content[index] = 0.0;
            if (!isfinite(this->ozone_mass_mixing_ratio[index])) this->ozone_mass_mixing_ratio[index] = 0.0;

            this->temperature[index] = this->thermal_energy[index] / (m * c_p);
            this->density[index] = m / this->volume[index];

            float e = this->calculateWaterVaporPressure(this->specificHumidity[index], pres);
            float T = this->temperature[index];
            float e_s = 611.2 * exp(17.67 * (T - 273.15) / (T - 29.65));

            this->relativeHumidity[index] = e / e_s;
            if (!isfinite(this->relativeHumidity[index])) this->relativeHumidity[index] = 0.0;

            // float RH_crit = 1.f;
            // float x = std::max(0.f, (this->relativeHumidity[index] - RH_crit) / (1.f - RH_crit));
            
            // this->cloud_cover[index] = x * x;

            float area = faceAreaTB[idx(nlon, nlat)];
            float lwp = cloud_liquid_mass[index] / area;

            float r_e = 10e-6f; // 10 µm droplets
            float tau = 3.f * lwp / (2.f * 1000.f * r_e);

            this->cloud_cover[index] = std::clamp(1.f - exp(-tau), 0.f, 1.f);

            if (!isfinite(this->cloud_cover[index])) this->cloud_cover[index] = 0.0;
          }
        }
      } });
  }
  this->threadPool.wait();

  std::cout << "MASS 0: " << this->mass[1000] << std::endl;
  std::cout << "MASS LIQUID 0: " << this->cloud_liquid_mass[1000] << std::endl;
  std::cout << "MASS ICE 0: " << this->cloud_ice_mass[1000] << std::endl;
  std::cout << "MASS ICE CONTENT 0: " << this->cloud_ice_water_content[1000] << std::endl;
  std::cout << "DENSITY 0: " << this->density[1000] << std::endl;
  std::cout << "ENERGY 0: " << this->thermal_energy[1000] << std::endl;
  std::cout << "TEMPERATURE 0: " << this->temperature[1000] << std::endl;
  std::cout << "WATER MASS 0: " << this->cloud_liquid_mass[1000] << std::endl;
  std::cout << "SPECIFIC HUMIDITY 0: " << this->specificHumidity[1000] << std::endl;
  std::cout << "RELATIVE HUMIDITY 0: " << this->relativeHumidity[1000] << std::endl;
  std::cout << "CLOUD COVER 0: " << this->cloud_cover[1000] << std::endl;
  std::cout << std::endl;
}

// Constructor
Atmosphere::Atmosphere(std::string &folderPath, ThreadPool &threadPool, double g, double radius) : threadPool(threadPool)
{
  this->initFromMetadata(folderPath);

  this->readFloatFile(folderPath + "/t.bin", this->temperature);
  this->readFloatFile(folderPath + "/r.bin", this->relativeHumidity);
  this->readFloatFile(folderPath + "/q.bin", this->specificHumidity);
  this->readFloatFile(folderPath + "/cc.bin", this->cloud_cover);
  this->readFloatFile(folderPath + "/u.bin", this->u_wind);
  this->readFloatFile(folderPath + "/v.bin", this->v_wind);
  this->readFloatFile(folderPath + "/w.bin", this->w_wind);
  this->readFloatFile(folderPath + "/z.bin", this->geopotential);
  // this->readFloatFile(folderPath + "/vo.bin", this->vorticity);
  // this->readFloatFile(folderPath + "/pv.bin", this->potential_vorticity);
  // this->readFloatFile(folderPath + "/d.bin", this->divergence);
  this->readFloatFile(folderPath + "/o3.bin", this->ozone_mass_mixing_ratio);
  this->readFloatFile(folderPath + "/ciwc.bin", this->cloud_ice_water_content);
  this->readFloatFile(folderPath + "/clwc.bin", this->cloud_liquid_water_content);
  this->readFloatFile(folderPath + "/crwc.bin", this->cloud_rain_water_content);
  this->readFloatFile(folderPath + "/cswc.bin", this->cloud_snow_water_content);

  this->threadPool.initRanges(this->threadRanges, this->longitude.size());

  this->initVectors(g, radius);

  this->initTextures();
}

// Public functions
void Atmosphere::step(double dt, double g)
{
  this->scratch.forEach([](Flux &flux)
                        { flux.forEach([](std::vector<double> &vec)
                                       { std::fill(vec.begin(), vec.end(), 0.f); }); });

  for (size_t i = 0; i < this->threadRanges.size(); i++)
  {
    Range &work = this->threadRanges[i];
    this->threadPool.enqueue([this, &work, dt, g]()
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

        glm::dvec3 fluxes(m_e, m_n, m_t);
        glm::uvec3 indices(index);

        if (m_e < 0)
          indices.x = east;
        if (m_n < 0 && north)
          indices.y = *north;
        if (m_t < 0 && top)
          indices.z = *top;

        this->scratch.mass.calculateFlux(index, fluxes, indices, this->mass, this->mass);
        this->scratch.cloud_ice_mass.calculateFlux(index, fluxes, indices, this->cloud_ice_mass, this->mass);
        this->scratch.cloud_liquid_mass.calculateFlux(index, fluxes, indices, this->cloud_liquid_mass, this->mass);
        this->scratch.cloud_snow_mass.calculateFlux(index, fluxes, indices, this->cloud_snow_mass, this->mass);
        this->scratch.cloud_rain_mass.calculateFlux(index, fluxes, indices, this->cloud_rain_mass, this->mass);
        this->scratch.water_vapor_mass.calculateFlux(index, fluxes, indices, this->water_vapor_mass, this->mass);
        this->scratch.ozone_mass.calculateFlux(index, fluxes, indices, this->ozone_mass, this->mass);
        this->scratch.energy.calculateFlux(index, fluxes, indices, this->thermal_energy, this->mass);
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

  this->applyDelta();

  this->updateTextures();
}