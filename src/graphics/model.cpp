#include "graphics/model.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"

#include "graphics/bindings/texture.h"

#include "graphics/state/scopedTexture.h"

#include "graphics/materials/material.h"

#include "resources/OBJloader.h"

#include <glm/gtc/matrix_transform.hpp>

// Constructor/Descructor
Model::Model(Material &material, Mesh &mesh, Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular)
{
  this->material = &material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;

  this->mesh = &mesh;
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);
}

Model::Model(const Model &model)
{
  this->material = model.material;
  this->overrideTextureDiffuse = model.overrideTextureDiffuse;
  this->overrideTextureSpecular = model.overrideTextureSpecular;
  this->mesh = model.mesh;
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);
}

Model::Model(Material &material, const std::string &OBJfile, Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular)
{
  this->material = &material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;

  std::vector<VertexPositionTexcoordNormalColor> vertices = loadOBJmodel(OBJfile);
  std::vector<GLuint> indices;

  this->mesh = new Mesh(&vertices, &indices, VertexLayout::Full);
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);
}

Model::Model(Mesh &mesh)
{
  this->mesh = &mesh;
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);
}

Model::~Model() = default;

// Public functions
void Model::render(Shader &shader)
{
  if (this->material)
    this->material->sendToShader(shader);

  // Render objects
  std::optional<ScopedTexture> diffuseScope;
  std::optional<ScopedTexture> specularScope;

  if (this->overrideTextureDiffuse)
    diffuseScope.emplace(*this->overrideTextureDiffuse,
                         TextureBindingPoints::Diffuse);

  if (this->overrideTextureSpecular)
    specularScope.emplace(*this->overrideTextureSpecular,
                          TextureBindingPoints::Specular);

  if (this->mesh)
    this->mesh->render();

  // Unbind everything
  glBindVertexArray(0);
}

void Model::renderInstanced(Shader &shader, Buffer *instanceVBO, size_t size, size_t count, size_t offset)
{
  if (this->material)
    this->material->sendToShader(shader);

  // Render objects
  std::optional<ScopedTexture> diffuseScope;
  std::optional<ScopedTexture> specularScope;

  if (this->overrideTextureDiffuse)
    diffuseScope.emplace(*this->overrideTextureDiffuse,
                         TextureBindingPoints::Diffuse);

  if (this->overrideTextureSpecular)
    specularScope.emplace(*this->overrideTextureSpecular,
                          TextureBindingPoints::Specular);

  if (this->mesh)
    this->mesh->renderInstanced(instanceVBO, size, count, offset);

  // Unbind everything
  glBindVertexArray(0);
}

bool Model::getIsTangent() const
{
  return this->mesh->getIsTangent();
};