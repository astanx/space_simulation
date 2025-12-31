#include "graphics/model.h"
#include "graphics/material.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

#include "resources/OBJloader.h"
#include <glm/gtc/matrix_transform.hpp>

// Private functions
void Model::updateUniforms(Shader *shader)
{
  this->material->sendToShader(*shader);
  shader->setMat4fv(this->modelMatrix, "ModelMatrix");
}

// Constructor/Descructor
Model::Model(glm::vec3 position, Material *material,
             std::vector<std::unique_ptr<Mesh>> &meshes,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular)
{
  this->position = position;
  this->material = material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;

  for (auto &mesh : meshes)
    this->meshes.push_back(std::move(mesh));

  this->updateModelMatrix();
}
Model::Model(glm::vec3 position, Material *material,
             const char *OBJfile,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular)
{
  this->position = position;
  this->material = material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;

  std::vector<Vertex> vertices = loadOBJmodel(OBJfile);

  Mesh *mesh = new Mesh(vertices.data(), vertices.size(), nullptr, 0, this->position, this->position);
  this->meshes.push_back(std::unique_ptr<Mesh>(mesh));
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
    mesh->scaleBy(scale);
  }
}

void Model::updateModelMatrix()
{
  this->modelMatrix = glm::mat4(1.0f);
  this->modelMatrix = glm::translate(this->modelMatrix, this->position);
}

void Model::setPosition(const glm::vec3 &newPosition)
{
  this->position = newPosition;
  this->updateModelMatrix();
}