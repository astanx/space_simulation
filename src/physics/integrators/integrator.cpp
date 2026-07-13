#include "physics/integrators/integrator.h"

#include "physics/integrators/integratable.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/systems/system.h"

// Private functions
void Integrator::splitObjectsSystems(std::vector<Integratable *> &objects, std::vector<Object *> &objectPointers, std::vector<System *> &systemPointers)
{
  for (Integratable *object : objects)
  {
    if (object->getIsSystem())
    {
      System *sys = dynamic_cast<System *>(object);
      if (sys)
         systemPointers.push_back(sys);
    }
    else
    {
      Object *obj = dynamic_cast<Object *>(object);
      if (obj)
        objectPointers.push_back(obj);
    }
  }
}

void Integrator::splitObjectsSystems(std::vector<Integratable *> &objects, std::vector<Object *> &objectPointers, std::vector<OrbitalObject *> &orbitalObjectsPointers, std::vector<System *> &systemPointers)
{
  for (Integratable *object : objects)
  {
    if (object->getIsSystem())
    {
      System *sys = dynamic_cast<System *>(object);
      if (sys)
        systemPointers.push_back(sys);
    }
    else
    {
      Object *obj = dynamic_cast<Object *>(object);
      if (obj)
      {
        OrbitalObject *orb = dynamic_cast<OrbitalObject *>(obj);
        if (orb)
          orbitalObjectsPointers.push_back(orb);
        else
          objectPointers.push_back(obj);
      }
    }
  }
}
