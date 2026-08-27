#include "render/queue/builder/renderQueueBuilder.h"

#include "debug/logger.h"

#include "render/world/data/renderDatabaseView.h"
#include "render/lod/lodResult.h"
#include "render/instanceManager.h"
#include "render/queue/renderQueue.h"
#include "render/frustum.h"
#include "render/modelSource.h"
#include "render/renderSystem.h"
#include "render/lod/manager/lodManager.h"
#include "render/lod/lodConstants.h"

#include "graphics/model.h"

#include "camera/camera.h"

#include <iostream>

// Private functions
void RenderQueueBuilder::buildEntity(const Entity &entity, const RenderDatabaseView &database, LODManager &lod, Frustum *frustum, FrameContext ctx, float fov)
{
  Transform transform = database.getTransform(entity);
  Radii radii = database.getRadii(entity);
  const Model *model = database.getModel(entity);

  LODResult res = lod.partitionObject(transform.position, model->getImportance(), radii, frustum, ctx.height, fov);
  this->submit(model, res, transform);
}

// Constructor / Destructor
RenderQueueBuilder::RenderQueueBuilder(const std::vector<Model *> &models)
{
  uint32_t maxId = 0;
  for (const Model *model : models)
    maxId = std::max(maxId, model->getID());

  this->groups.resize(maxId + 1);

  for (const Model *model : models)
    this->groups[model->getID()] = RenderGroup{model};
}

// Public functions
void RenderQueueBuilder::build(RenderQueue &queue, const RenderDatabaseView &database, LODManager &lod, InstanceManager &instance, FrameContext &ctx)
{
  queue.clear();
  instance.clear();

  const Camera &camera = database.getCamera();
  float fov = camera.getFOV();
  Frustum frustum = camera.getFrustum(ctx.aspect);

  // fix here add threadPool
  for (const std::unique_ptr<Entity> &entity : database.getEntities())
  {
    if (!entity)
    {
      Logger::logError("Render Queue Builder", "Uninitialized entity detected");
      continue;
    }
    this->buildEntity(*entity, database, lod, &frustum, ctx, fov);
  }

  this->finish(instance, queue);

  instance.fillVBOs();
}

void RenderQueueBuilder::submit(const Model *model, const LODResult &lod, const Transform &transform)
{
  InstanceModelMatrixParts fullInstanceData;
  fullInstanceData.position = transform.position;
  fullInstanceData.orientation = transform.orientation;
  fullInstanceData.scale = glm::vec3(lod.equatorianScale, lod.polarScale, lod.equatorianScale);

  // Shadow pass & Reflector pass
  if (lod.level != LOD_FULL && (model->hasFlag(ModelFlags::CastsShadow) || model->hasFlag(ModelFlags::ReflectsLight)))
    this->groups[model->getID()].fullNonLODInstances.push_back(fullInstanceData);

  // LOD pass
  if (lod.visible)
    switch (lod.level)
    {
    case LOD_FULL:
    {
      this->groups[model->getID()].fullLODInstances.push_back(fullInstanceData);
      break;
    }

    case LOD_IMPOSTOR:
    {
      InstancePositionRadiusTexture data;
      data.position = transform.position;
      data.radius = lod.scaledMeanRadius;
      data.textureLayer = model->getImpostorLayer();

      this->impostors.push_back(data);
      break;
    }

    case LOD_POINT:
    {
      InstancePositionRadiusColor data;
      data.position = transform.position;
      data.radius = lod.scaledMeanRadius;
      data.color = model->getAverageColor();

      this->points.push_back(data);
      break;
    }

    default:
      Logger::logError("Render Queue Builder", "No handler for LOD level: " + std::to_string(lod.level));
      break;
    }
};

void RenderQueueBuilder::merge(RenderQueueBuilder &builder)
{
  for (size_t i = 0; i < groups.size(); i++)
  {
    if (this->groups[i].model != builder.groups[i].model)
      Logger::logFatal("Render Queue Builder", "Merging different types of queues");

    this->groups[i].fullLODInstances.insert(this->groups[i].fullLODInstances.end(), std::make_move_iterator(builder.groups[i].fullLODInstances.begin()), std::make_move_iterator(builder.groups[i].fullLODInstances.end()));
    this->groups[i].fullNonLODInstances.insert(this->groups[i].fullNonLODInstances.end(), std::make_move_iterator(builder.groups[i].fullNonLODInstances.begin()), std::make_move_iterator(builder.groups[i].fullNonLODInstances.end()));
  }

  impostors.insert(impostors.end(), std::make_move_iterator(builder.impostors.begin()), std::make_move_iterator(builder.impostors.end()));
  points.insert(points.end(), std::make_move_iterator(builder.points.begin()), std::make_move_iterator(builder.points.end()));
}

void RenderQueueBuilder::finish(InstanceManager &instances, RenderQueue &queue)
{
  for (auto &group : this->groups)
  {
    if (!group.model)
      continue;

    size_t lodSize = group.fullLODInstances.size();

    group.fullLODInstances.insert(group.fullLODInstances.end(), std::make_move_iterator(group.fullNonLODInstances.begin()), std::make_move_iterator(group.fullNonLODInstances.end()));
    Range fullRange = instances.add(group.model, std::move(group.fullLODInstances));

    Range fullLODRange;
    fullLODRange.begin = fullRange.begin;
    fullLODRange.end = fullRange.begin + lodSize;

    if (lodSize > 0)
    {
      if (group.model->getIsTangent())
        queue.addTangentBatch({group.model, fullLODRange});
      else
        queue.addCoreBatch({group.model, fullLODRange});
    }
  }

  instances.add(std::move(this->impostors));
  instances.add(std::move(this->points));
}