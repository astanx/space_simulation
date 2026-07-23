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
AsteroidType &ResourceManager::LoadAsteroid(const std::string &name, const std::string &model_name, const std::string &mesh_name, std::unique_ptr<AsteroidShape> shape, Material &material, VertexLayout layout)
{
  Radii radii = shape->getRadii();
  double volume = shape->calculateVolume();
  Mesh &mesh = this->LoadMesh<T>(mesh_name, std::move(shape), layout);
  Model &model = this->LoadModel(model_name, material, mesh);

  this->asteroids[name] = std::make_unique<AsteroidType>(name, &model, radii, volume);
  return *this->asteroids[name];
}