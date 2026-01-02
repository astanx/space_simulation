#include "graphics/model.h"
#include "graphics/material.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

#include "resources/OBJloader.h"
#include <glm/gtc/matrix_transform.hpp>

// Private functions
void Model::updateUniforms(Shader *shader)
{
  if (this->material)
    this->material->sendToShader(*shader);
}

// Constructor/Descructor
Model::Model(glm::vec3 position, Material *material,
             std::vector<Mesh *> meshes,

             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular,
             glm::vec3 rotation, glm::vec3 scale)
{
  this->position = position;
  this->material = material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;
  this->rotation = rotation;
  this->scale = scale;

  for (auto &mesh : meshes)
  {
    this->meshes.push_back(mesh);
    mesh->setScale(scale);
    mesh->rotate(rotation);
  }
}

Model::Model(glm::vec3 position, Material *material,
             Mesh *mesh,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular,
             glm::vec3 rotation, glm::vec3 scale)
{
  this->position = position;
  this->material = material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;
  this->rotation = rotation;
  this->scale = scale;

  mesh->setScale(scale);
  mesh->rotate(rotation);
  this->meshes.push_back(mesh);
}

Model::Model(glm::vec3 position, Material *material,
             const char *OBJfile,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular,
             glm::vec3 rotation, glm::vec3 scale)
{
  this->position = position;
  this->material = material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;
  this->rotation = rotation;
  this->scale = scale;

  std::vector<Vertex> vertices = loadOBJmodel(OBJfile);

  Mesh *mesh = new Mesh(vertices.data(), vertices.size(), nullptr, 0, this->position, this->position);
  mesh->setScale(scale);
  mesh->rotate(rotation);
  this->meshes.push_back(mesh);
}

Model::~Model()
{
}

// Public functions
void Model::render(Shader *shader)
{
  shader->use();
  // Update uniforms
  this->updateUniforms(shader);

  // Render objects
  if (this->overrideTextureDiffuse != nullptr)
    this->overrideTextureDiffuse->bind(0);
  if (this->overrideTextureSpecular != nullptr)
    this->overrideTextureSpecular->bind(1);

  for (auto &mesh : this->meshes)
  {
    mesh->setPosition(this->position);
    mesh->render(shader);
  }

  // Unbind everything
  glBindVertexArray(0);
  if (this->overrideTextureDiffuse != nullptr)
    this->overrideTextureDiffuse->unbind();
  if (this->overrideTextureSpecular != nullptr)
    this->overrideTextureSpecular->unbind();

  shader->unuse();
}
void Model::scaleBy(const glm::vec3 scale)
{
  for (auto &mesh : meshes)
  {
    mesh->setScale(scale);
  }
}

void Model::setPosition(const glm::vec3 &newPosition)
{
  this->position = newPosition;
}