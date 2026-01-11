#include "graphics/model.h"
#include "graphics/materials/material.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

#include "resources/OBJloader.h"
#include <glm/gtc/matrix_transform.hpp>

// Private functions
void Model::updateUniforms(Shader *shader)
{
  if (this->material)
    this->material->sendToShader(*shader);
  shader->setMat4fv(this->ModelMatrix, "ModelMatrix");
  shader->set1i(1, "useModelMatrix");
}

void Model::updateModelMatrix()
{
  this->ModelMatrix = glm::mat4(1.f);
  this->ModelMatrix = glm::translate(this->ModelMatrix, this->rotationOrigin);
  this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
  this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.y), glm::vec3(0.f, 1.f, 0.f));
  this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
  this->ModelMatrix = glm::translate(this->ModelMatrix, this->position - this->rotationOrigin);
  this->ModelMatrix = glm::scale(this->ModelMatrix, this->scale);
}
// Constructor/Descructor
Model::Model(glm::vec3 position, Material *material,
             std::vector<Mesh *> meshes,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular,
             glm::vec3 rotation, glm::vec3 scale, glm::vec3 rotationOrigin)
{
  this->position = position;
  this->material = material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;
  this->rotation = rotation;
  this->scale = scale;
  this->rotationOrigin =
      (rotationOrigin == glm::vec3(0.f))
          ? position
          : rotationOrigin;

  for (auto &mesh : meshes)
  {
    this->meshes.push_back(mesh);
  }
}

Model::Model(glm::vec3 position, Material *material,
             Mesh *mesh,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular,
             glm::vec3 rotation, glm::vec3 scale, glm::vec3 rotationOrigin)
{
  this->position = position;
  this->material = material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;
  this->rotation = rotation;
  this->scale = scale;
  this->rotationOrigin =
      (rotationOrigin == glm::vec3(0.f))
          ? position
          : rotationOrigin;

  this->meshes.push_back(mesh);
}

Model::Model(const Model &model)
{
  this->position = model.position;
  this->material = model.material;
  this->overrideTextureDiffuse = model.overrideTextureDiffuse;
  this->overrideTextureSpecular = model.overrideTextureSpecular;
  this->rotation = model.rotation;
  this->scale = model.scale;
  this->rotationOrigin = model.rotationOrigin;
  this->meshes = model.meshes;
}

Model::Model(glm::vec3 position, Material *material,
             const char *OBJfile,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular,
             glm::vec3 rotation, glm::vec3 scale, glm::vec3 rotationOrigin)
{
  this->position = position;
  this->material = material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;
  this->rotation = rotation;
  this->scale = scale;
  this->rotationOrigin =
      (rotationOrigin == glm::vec3(0.f))
          ? position
          : rotationOrigin;

  std::vector<Vertex> vertices = loadOBJmodel(OBJfile);

  Mesh *mesh = new Mesh(vertices.data(), vertices.size(), nullptr, 0, VertexLayout::Full);
  this->meshes.push_back(mesh);
}

Model::~Model()
{
}

// Public functions
void Model::render(Shader *shader)
{
  // Update uniforms
  this->updateModelMatrix();
  this->updateUniforms(shader);

  // Render objects
  if (this->overrideTextureDiffuse != nullptr)
    this->overrideTextureDiffuse->bind(0);
  if (this->overrideTextureSpecular != nullptr)
    this->overrideTextureSpecular->bind(1);

  for (auto &mesh : this->meshes)
  {
    mesh->render();
  }

  // Unbind everything
  glBindVertexArray(0);
  if (this->overrideTextureDiffuse != nullptr)
    this->overrideTextureDiffuse->unbind();
  if (this->overrideTextureSpecular != nullptr)
    this->overrideTextureSpecular->unbind();
}

void Model::renderInstanced(Shader *shader)
{
  // Update uniforms
  // this->updateModelMatrix();
  // this->updateUniforms(shader);

  // Render objects
  // if (this->overrideTextureDiffuse != nullptr)
  //   this->overrideTextureDiffuse->bind(0);
  // if (this->overrideTextureSpecular != nullptr)
  //   this->overrideTextureSpecular->bind(1);

  for (auto &mesh : this->meshes)
  {
    mesh->renderInstanced();
  }

  // Unbind everything
  glBindVertexArray(0);
  // if (this->overrideTextureDiffuse != nullptr)
  //   this->overrideTextureDiffuse->unbind();
  // if (this->overrideTextureSpecular != nullptr)
  //   this->overrideTextureSpecular->unbind();
}

void Model::scaleBy(const glm::vec3 &scale)
{
  this->scale *= scale;
}

glm::vec3 Model::getPosition() const
{
  return this->position;
}

void Model::rotate(const glm::vec3 &rotation)
{
  this->rotation += rotation;
}

void Model::setPosition(const glm::vec3 &newPosition)
{
  this->position = newPosition;
}

void Model::setRotationOrigin(const glm::vec3 &newRotationOrigin)
{
  this->rotationOrigin = newRotationOrigin;
}