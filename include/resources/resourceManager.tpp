#pragma once

#include "graphics/mesh.h"

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