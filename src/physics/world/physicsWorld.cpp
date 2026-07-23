#include "physics/world/physicsWorld.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/planet.h"
#include "physics/star.h"
#include "physics/systems/asteroidSystem.h"

#include "physics/integrators/integratorCPU.h"
#include "physics/integrators/integratorGPU.h"

#include "physics/integrators/wisdomHolmanCPU.h"
#include "physics/integrators/wisdomHolmanGPU.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include "debug/logger.h"

// Private functions
template <typename Real>
void PhysicsWorld::processObject(Object *obj, DataGPU<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex)
{
  data.positions[i] = static_cast<Vec3<Real>>(obj->getPosition());
  data.velocities[i] = static_cast<Vec3<Real>>(obj->getVelocity());
  data.mus[i] = static_cast<Real>(obj->getMu());
  data.meanRadii[i] = static_cast<Real>(obj->getRadius());

  data.orientations[i] = static_cast<Quat<Real>>(obj->getOrientation());
  data.angularVelocities[i] = static_cast<Vec3<Real>>(obj->getAngularVelocity());

  data.centralBodyIndices[i] = -1;

  data.tensors[i] = static_cast<Mat3<Real>>(obj->getQuadrupoleTensor());

  data.loveIndices[i] = -1;
  data.tidalFactorIndices[i] = -1;

  const TidalParameters &p = obj->getTidalParameters();
  if (p.k2 != -1)
  {
    std::lock_guard<std::mutex> lock(loveMutex);

    loveNumbers.push_back(static_cast<Real>(p.k2));
    data.loveIndices[i] = loveNumbers.size() - 1;
  }
  if (p.Q != -1)
  {
    std::lock_guard<std::mutex> lock(tidalMutex);

    tidalFactors.push_back(static_cast<Real>(p.Q));
    data.tidalFactorIndices[i] = tidalFactors.size() - 1;
  }

  // data.instanceTextureLayer;
  // data.instanceScale;
  // data.instanceColor;
};

template <typename Real>
void PhysicsWorld::processOrbital(OrbitalObject *obj, DataGPU<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex)
{
  this->processObject(obj, data, i, loveNumbers, tidalFactors, loveMutex, tidalMutex);

  const KeplerElements k = obj->getOrbit()->getKeplerElements();

  data.semiAxises[i] = static_cast<Real>(k.a);
  data.eccentricities[i] = static_cast<Real>(k.e);
  data.inclinations[i] = static_cast<Real>(k.i);
  data.longitude[i] = static_cast<Real>(k.Omega);
  data.periapsis[i] = static_cast<Real>(k.omega);
  data.meanAnomaly[i] = static_cast<Real>(k.m);
  data.meanMotion[i] = static_cast<Real>(k.n);
};
template <typename Real>
void PhysicsWorld::initGPUBuffers(Context &context)
{
  std::vector<OrbitalObject *> orbitalObjects;
  std::vector<Object *> objects;

  for (Object *object : this->cpu.integratableObjects)
  {
    OrbitalObject *orb = dynamic_cast<OrbitalObject *>(object);
    if (orb)
      orbitalObjects.push_back(orb);
    else
      objects.push_back(object);
  }

  this->total.orbital = orbitalObjects.size();

  for (System *sys : this->cpu.integratableSystems)
  {
    std::atomic_size_t sysOrbital = 0;

    sys->forEachObject([&sysOrbital](Object &obj, size_t i)
                       { 
                        if (dynamic_cast<OrbitalObject *>(&obj) != nullptr) 
                          sysOrbital.fetch_add(1); });

    this->total.orbital += sysOrbital;
  }

  Logger::logInfo("PhysicsWorld", "calculated total");
  this->total.total = objects.size() + orbitalObjects.size();

  for (System *sys : this->cpu.integratableSystems)
    this->total.total += sys->getTotalObjects();

  DataGPU<Real> objectGPU;
  DataGPU<Real> orbitalGPU;

  this->total.object = this->total.total - this->total.orbital;
  objectGPU.resize(this->total.object);
  orbitalGPU.resize(this->total.total);

  size_t orbitalOffset = 0;
  size_t objectOffset = 0;

  std::mutex loveMutex;
  std::mutex tidalMutex;
  std::vector<Real> loveNumbers;
  std::vector<Real> tidalFactors;

  for (OrbitalObject *obj : orbitalObjects)
    this->processOrbital(obj, orbitalGPU, orbitalOffset++, loveNumbers, tidalFactors, loveMutex, tidalMutex);

  for (Object *obj : objects)
    this->processObject(obj, objectGPU, objectOffset++, loveNumbers, tidalFactors, loveMutex, tidalMutex);

  for (size_t i = 0; i < orbitalObjects.size(); i++)
  {
    Object *central = orbitalObjects[i]->getOrbit()->getCentralBody();
    for (size_t j = 0; j < objects.size(); j++)
    {
      if (objects[j] == central)
        orbitalGPU.centralBodyIndices[i] = j + this->total.orbital;
    }

    for (size_t j = 0; j < orbitalObjects.size(); j++)
    {
      if (orbitalObjects[j] == central)
        orbitalGPU.centralBodyIndices[i] = j;
    }
  }
  Logger::logInfo("PhysicsWorld", "processes obj");

  std::atomic_size_t orbitalIndex{orbitalOffset};
  std::atomic_size_t objectIndex{objectOffset};
  for (System *sys : this->cpu.integratableSystems)
    sys->forEachObject([this, &orbitalGPU, &objectGPU, &orbitalIndex, &objectIndex, &loveNumbers, &tidalFactors, &loveMutex, &tidalMutex, &objects, &orbitalObjects](Object &obj)
                       {
      OrbitalObject* orb = dynamic_cast<OrbitalObject*>(&obj);
      if (orb)
      {
        size_t idx = orbitalIndex.fetch_add(1);
        this->processOrbital(orb, orbitalGPU, idx, loveNumbers, tidalFactors, loveMutex, tidalMutex);

        Object* central = orb->getOrbit()->getCentralBody();
        for (size_t j = 0; j < objects.size(); j++)
        {
          if (objects[j] == central)
            orbitalGPU.centralBodyIndices[idx] = j + this->total.orbital;
        }

        for (size_t j = 0; j < orbitalObjects.size(); j++)
        {
          if (orbitalObjects[j] == central)
            orbitalGPU.centralBodyIndices[idx] = j;
        }
      }
      else
      { 
        size_t idx = objectIndex.fetch_add(1);
        this->processObject(&obj, objectGPU, idx, loveNumbers, tidalFactors, loveMutex, tidalMutex); 
      } });

  Logger::logInfo("PhysicsWorld", "processed sys");

  // orbital MUST be first
  orbitalGPU.combine(objectGPU);

  Logger::logInfo("PhysicsWorld", "combined");

  cl_context ctx = context.get();
  this->gpu.positionsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.positions.size() * sizeof(Vec3<Real>), orbitalGPU.positions.data());
  this->gpu.musBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.mus.size() * sizeof(Real), orbitalGPU.mus.data());
  this->gpu.velocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.velocities.size() * sizeof(Vec3<Real>), orbitalGPU.velocities.data());
  this->gpu.meanRadiiBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.meanRadii.size() * sizeof(Real), orbitalGPU.meanRadii.data());
  // temp
  this->gpu.polarRadiiBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.meanRadii.size() * sizeof(Real), orbitalGPU.meanRadii.data());
  this->gpu.equatorianRadiiBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.meanRadii.size() * sizeof(Real), orbitalGPU.meanRadii.data());

  this->gpu.orientationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.orientations.size() * sizeof(Quat<Real>), orbitalGPU.orientations.data());
  this->gpu.angularVelocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.angularVelocities.size() * sizeof(Vec3<Real>), orbitalGPU.angularVelocities.data());

  this->gpu.semiAxisesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.semiAxises.size() * sizeof(Real), orbitalGPU.semiAxises.data());
  this->gpu.eccentricitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.eccentricities.size() * sizeof(Real), orbitalGPU.eccentricities.data());
  this->gpu.inclinationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.inclinations.size() * sizeof(Real), orbitalGPU.inclinations.data());
  this->gpu.longitudeBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.longitude.size() * sizeof(Real), orbitalGPU.longitude.data());
  this->gpu.periapsisBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.periapsis.size() * sizeof(Real), orbitalGPU.periapsis.data());
  this->gpu.meanAnomalyBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.meanAnomaly.size() * sizeof(Real), orbitalGPU.meanAnomaly.data());
  this->gpu.meanMotionBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.meanMotion.size() * sizeof(Real), orbitalGPU.meanMotion.data());
  this->gpu.centralBodyIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.centralBodyIndices.size() * sizeof(int), orbitalGPU.centralBodyIndices.data());

  this->gpu.tensorsBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.tensors.size() * sizeof(Mat3<Real>), orbitalGPU.tensors.data());
  this->gpu.loveIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.loveIndices.size() * sizeof(int), orbitalGPU.loveIndices.data());
  this->gpu.tidalFactorIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.tidalFactorIndices.size() * sizeof(int), orbitalGPU.tidalFactorIndices.data());
  this->gpu.loveNumbersBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, loveNumbers.size() * sizeof(Real), loveNumbers.data());
  this->gpu.tidalFactorsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, tidalFactors.size() * sizeof(Real), tidalFactors.data());
}

// Constructor / Destructor
PhysicsWorld::PhysicsWorld()
{
  this->integratorCPU = std::make_unique<WisdomHolmanIntegratorCPU>();
}
PhysicsWorld::~PhysicsWorld() = default;

// Public functions
void PhysicsWorld::initGPU(ResourceManager &resourceManager, Context &ctx)
{
  // bool supportsDouble = ctx.getSupportsDouble();
  // if (supportsDouble)
  // {
  //   this->initGPUBuffers<double>(ctx);
  //   this->integratorGPU = std::make_unique<WisdomHolmanIntegratorGPU<double>>(resourceManager);
  // }
  // else
  // {
  //   this->initGPUBuffers<float>(ctx);
  //   this->integratorGPU = std::make_unique<WisdomHolmanIntegratorGPU<float>>(resourceManager);
  // }

  // this->integratorGPU->init(this->gpu, this->total, ctx);
}
void PhysicsWorld::step(double dt)
{
  this->integratorCPU->step(this->cpu.integratableObjects, this->cpu.integratableSystems, dt);
  // this->integratorGPU->step(this->total, dt);
}

void PhysicsWorld::addObject(Object *object)
{
  this->cpu.objects.push_back(object);
}
void PhysicsWorld::addAtmosphere(Atmosphere *atmosphere)
{
  this->cpu.atmospheres.push_back(atmosphere);
}
void PhysicsWorld::addPlanetarObject(std::unique_ptr<Planet> planetarObject)
{
  this->cpu.planetarObjectViews.push_back(planetarObject.get());
  this->cpu.planetarObjects.push_back(std::move(planetarObject));
}
void PhysicsWorld::addIntegratable(Integratable *object)
{
  std::vector<Object *> integratableObjects;
  std::vector<System *> integratableSystems;

  Object *obj = dynamic_cast<Object *>(object);
  if (obj)
  {
    this->cpu.integratableObjects.push_back(obj);
    return;
  }

  System *sys = dynamic_cast<System *>(object);
  if (sys)
  {
    this->cpu.integratableSystems.push_back(sys);
    return;
  }

  this->cpu.integratable.push_back(object);
}
void PhysicsWorld::addSun(Star *sun)
{
  if (this->sun)
    Logger::logFatal("PhysicsWorld", "Sun already exists");

  this->sun = sun;
}
void PhysicsWorld::addStar(std::unique_ptr<Star> star)
{
  this->cpu.stars.push_back(std::move(star));
}
void PhysicsWorld::addAsteroidSystem(std::unique_ptr<AsteroidSystem> asteroidSystem)
{
  this->cpu.asteroidSystemViews.push_back(asteroidSystem.get());
  this->cpu.asteroidSystems.push_back(std::move(asteroidSystem));
}
void PhysicsWorld::addSystem(System *system)
{
  this->cpu.systems.push_back(system);
}
const Star &PhysicsWorld::getSun() const
{
  if (!this->sun)
    Logger::logFatal("PhysicsWorld", "No sun");

  return *this->sun;
};
const std::vector<AsteroidSystem *> &PhysicsWorld::getAsteroidSystems() const
{
  if (this->cpu.asteroidSystemViews.empty())
    Logger::logWarning("PhysicsWorld", "Asteroid systems are empty");

  return this->cpu.asteroidSystemViews;
};
const std::vector<Planet *> &PhysicsWorld::getPlanetarObjects() const
{
  if (this->cpu.planetarObjectViews.empty())
    Logger::logWarning("PhysicsWorld", "Planets are empty");

  return this->cpu.planetarObjectViews;
};

const std::vector<Object *> &PhysicsWorld::getObjects() const
{
  return this->cpu.objects;
};

const std::vector<System *> &PhysicsWorld::getSystems() const
{
  return this->cpu.systems;
};