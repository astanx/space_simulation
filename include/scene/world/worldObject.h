#pragma once

class Object;
class OrbitalObject;
class System;
class Model;
class RenderSystem;

struct WorldObject
{
  Object *physics;
  Model *render;
};

struct WorldOrbitalObject
{
  OrbitalObject *physics;
  Model *render;
};

struct WorldSystem
{
  System *physics;
  RenderSystem *render;
};