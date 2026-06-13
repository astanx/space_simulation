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
  shader.setMat4fv(this->ModelMatrix, "ModelMatrix");
}

void Model::updateModelMatrix()
{
  this->ModelMatrix = glm::mat4(1.f);
  this->ModelMatrix = glm::translate(this->ModelMatrix, this->position);
  this->ModelMatrix *= glm::mat4(this->orientation);
  this->ModelMatrix = glm::scale(this->ModelMatrix, this->scale);
}
// Constructor/Descructor
Model::Model(glm::vec3 position, Material &material,
             std::vector<Mesh *> meshes,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular,
             glm::mat3 orientation, glm::vec3 scale)
{
  this->position = position;
  this->material = &material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;
  this->orientation = orientation;
  this->scale = scale;

  for (Mesh *&mesh : meshes)
    this->meshes.push_back(mesh);
}

Model::Model(glm::vec3 position, Material &material,
             Mesh &mesh,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular,
             glm::mat3 orientation, glm::vec3 scale)
{
  this->position = position;
  this->material = &material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;
  this->orientation = orientation;
  this->scale = scale;

  this->meshes.push_back(&mesh);
}

Model::Model(const Model &model)
{
  this->position = model.position;
  this->material = model.material;
  this->overrideTextureDiffuse = model.overrideTextureDiffuse;
  this->overrideTextureSpecular = model.overrideTextureSpecular;
  this->orientation = model.orientation;
  this->scale = model.scale;
  this->meshes = model.meshes;
}

Model::Model(glm::vec3 position, Material &material,
             const std::string &OBJfile,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular,
             glm::mat3 orientation, glm::vec3 scale)
{
  this->position = position;
  this->material = &material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;
  this->orientation = orientation;
  this->scale = scale;

  std::vector<Vertex> vertices = loadOBJmodel(OBJfile);
  std::vector<GLuint> indices;

  Mesh *mesh = new Mesh(&vertices, &indices, VertexLayout::Full);
  this->meshes.push_back(mesh);
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

  // Render objects
  std::optional<ScopedTexture> diffuseScope;
  std::optional<ScopedTexture> specularScope;

  if (this->overrideTextureDiffuse)
    diffuseScope.emplace(*this->overrideTextureDiffuse,
                         TextureBindingPoints::Diffuse);

  if (this->overrideTextureSpecular)
    specularScope.emplace(*this->overrideTextureSpecular,
                          TextureBindingPoints::Specular);

  for (Mesh *&mesh : this->meshes)
    mesh->render();

  // Unbind everything
  glBindVertexArray(0);
}

void Model::renderInstanced()
{
  for (Mesh *&mesh : this->meshes)
    mesh->renderInstanced();

  // Unbind everything
  glBindVertexArray(0);
}

void Model::scaleBy(const glm::vec3 &scale)
{
  this->scale *= scale;
}

glm::vec3 Model::getPosition() const
{
  return this->position;
}
glm::mat3 Model::getOrientation() const
{
  return this->orientation;
}

void Model::setOrientation(const glm::mat3 &orientation)
{
  this->orientation = orientation;
}

void Model::setPosition(const glm::vec3 &newPosition)
{
  this->position = newPosition;
}