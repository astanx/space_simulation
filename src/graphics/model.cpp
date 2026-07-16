#include "graphics/model.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

#include "graphics/bindings/texture.h"

#include "graphics/state/scopedTexture.h"

#include "graphics/materials/material.h"

#include "resources/OBJloader.h"

#include <glm/gtc/matrix_transform.hpp>

// Private functions
void Model::updateUniforms(Shader &shader)
{
  if (this->material)
    this->material->sendToShader(shader);
}

void Model::updateModelMatrix()
{
  this->modelMatrix = glm::mat4(1.f);
  this->modelMatrix = glm::translate(this->modelMatrix, this->position);
  this->modelMatrix *= glm::mat4(this->orientation);
  this->modelMatrix = glm::scale(this->modelMatrix, this->scale);
}
// Constructor/Descructor
Model::Model(glm::vec3 position, Material &material,
             Mesh &mesh,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular,
             glm::quat orientation, glm::vec3 scale)
{
  this->position = position;
  this->material = &material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;
  this->orientation = orientation;
  this->scale = scale;

  this->mesh = &mesh;
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);
}

Model::Model(const Model &model)
{
  this->position = model.position;
  this->material = model.material;
  this->overrideTextureDiffuse = model.overrideTextureDiffuse;
  this->overrideTextureSpecular = model.overrideTextureSpecular;
  this->orientation = model.orientation;
  this->scale = model.scale;
  this->mesh = model.mesh;
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);
}

Model::Model(glm::vec3 position, Material &material,
             const std::string &OBJfile,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular,
             glm::quat orientation, glm::vec3 scale)
{
  this->position = position;
  this->material = &material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;
  this->orientation = orientation;
  this->scale = scale;

  std::vector<VertexPositionTexcoordNormalColor> vertices = loadOBJmodel(OBJfile);
  std::vector<GLuint> indices;

  this->mesh = new Mesh(&vertices, &indices, VertexLayout::Full);
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);
}

Model::Model(glm::vec3 position, Mesh &mesh)
{
  this->position = position;
  this->mesh = &mesh;
  this->mesh->setInstanceLayout(InstanceLayout::ModelMatrixParts);
}

Model::~Model()
{
}

// Public functions
void Model::render(Shader &shader)
{
  // Update uniforms
  this->updateModelMatrix();
  this->updateUniforms(shader);

  shader.setMat4fv(this->modelMatrix, "ModelMatrix");

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

void Model::renderInstanced(Shader &shader)
{
  // Update uniforms
  this->updateUniforms(shader);

  std::vector<InstanceModelMatrixParts> buf;
  buf.emplace_back(InstanceModelMatrixParts{this->position, this->orientation, this->scale});

  this->mesh->setInstanceBuffer(buf.data(), buf.size());

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
    this->mesh->renderInstanced();

  // Unbind everything
  glBindVertexArray(0);
}

void Model::setScale(const glm::vec3 &scale)
{
  this->scale = scale;
}

glm::vec3 Model::getPosition() const
{
  return this->position;
}

glm::quat Model::getOrientation() const
{
  return this->orientation;
}

bool Model::getIsTangent() const
{
  return this->mesh->getIsTangent();
};

void Model::setOrientation(const glm::quat &orientation)
{
  this->orientation = orientation;
}

void Model::setPosition(const glm::vec3 &newPosition)
{
  this->position = newPosition;
}
