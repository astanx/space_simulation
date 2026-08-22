#include "graphics/model.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"

#include "graphics/bindings/texture.h"

#include "graphics/state/scopedTexture.h"

#include "graphics/materials/material.h"

#include "resources/OBJloader.h"

#include <glm/gtc/matrix_transform.hpp>

// Constructor/Descructor
Model::Model(Material &material, Mesh &mesh, ModelFlags flags, Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular)
{
  this->material = &material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;

  this->mesh = &mesh;
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);

  this->flags = flags;
}

Model::Model(const Model &model)
{
  this->material = model.material;
  this->flags = model.flags;
  this->overrideTextureDiffuse = model.overrideTextureDiffuse;
  this->overrideTextureSpecular = model.overrideTextureSpecular;
  this->mesh = model.mesh;
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);
}

Model::Model(Material &material, const std::string &OBJfile, ModelFlags flags, Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular)
{
  this->material = &material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;

  std::vector<VertexPositionTexcoordNormalColor> vertices = loadOBJmodel(OBJfile);
  std::vector<GLuint> indices;

  this->mesh = new Mesh(&vertices, &indices, VertexLayout::Full);
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);

  this->flags = flags;
}

Model::Model(Mesh &mesh, ModelFlags flags)
{
  this->mesh = &mesh;
  this->flags = flags;
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);
}

Model::~Model() = default;

// Public functions
void Model::render(Shader &shader) const
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

void Model::renderInstanced(Shader &shader, Buffer *instanceVBO, size_t size, size_t count, size_t offset) const
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

const glm::vec3 &Model::getAverageColor() const
{
  if (!this->material)
    Logger::logFatal("Model", "No material to get average color");

  const Texture *text = this->material->getTexture();

  if (!text)
    Logger::logFatal("Model", "No texture to get average color");

  return text->getAverageColor();
}
