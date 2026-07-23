#pragma once

#include "graphics/mesh.h"

#include "graphics/primitives/asteroidShape.h"

#include <vector>
#include <string>

template <typename T>
Mesh &ResourceManager::LoadMesh(const std::string &name, std::vector<T> *vertexArray, std::vector<GLuint> *indexArray, VertexLayout layout, GLenum drawMode)
{
  this->meshes[name] = std::make_unique<Mesh>(vertexArray, indexArray, layout, drawMode);
  return *this->meshes[name];
}

template <typename T>
Mesh &ResourceManager::LoadMesh(const std::string &name, std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode)
{
  this->meshes[name] = std::make_unique<Mesh>(TypeTag<T>{}, std::move(primitive), layout, drawMode);
  return *this->meshes[name];
}

template <typename T>
AsteroidType &ResourceManager::LoadAsteroid(const std::string &name, std::unique_ptr<AsteroidShape> shape, Material &material, VertexLayout layout)
{
  Radii radii = shape->getRadii();
  double volume = shape->calculateVolume();
  Mesh &mesh = this->LoadMesh<T>(name, std::move(shape), layout);

  this->asteroids[name] = std::make_unique<AsteroidType>(name, std::make_unique<Model>(material, mesh), radii, volume);
  return *this->asteroids[name];
}