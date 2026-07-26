#pragma once

class Object;
class OrbitalObject;
class System;
class ModelSource;
class RenderSystem;

struct WorldObject
{
  Object *physics;
  ModelSource *render;
};

struct WorldOrbitalObject
{
  OrbitalObject *physics;
  ModelSource *render;
};

struct WorldSystem
{
  System *physics;
  RenderSystem *render;
};