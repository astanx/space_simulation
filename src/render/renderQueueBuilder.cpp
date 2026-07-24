#include "render/renderQueueBuilder.h"

#include "debug/logger.h"

#include "render/lodResult.h"
#include "render/instanceManager.h"
#include "render/renderQueue.h"

#include "graphics/model.h"

// Constructor / Destructor
RenderQueueBuilder::RenderQueueBuilder(std::vector<Model *> models)
{
  for (size_t i = 0; i < models.size(); i++)
    this->groups.push_back(RenderGroup{models[i]});
}

// Public functions
void RenderQueueBuilder::submit(Model *model, const LODResult &lod, const Transform &transform)
{
  InstanceModelMatrixParts fullInstanceData;
  fullInstanceData.position = transform.position;
  fullInstanceData.orientation = transform.orientation;
  fullInstanceData.scale = glm::vec3(lod.equatorianScale, lod.polarScale, lod.equatorianScale);

  // Shadow pass
  if (model->hasFlag(ModelFlags::CastsShadow))
    this->groups[model->getQueueIndex()].fullShadowInstances.push_back(fullInstanceData);

  // Reflector pass
  if (model->hasFlag(ModelFlags::ReflectsLight))
    this->groups[model->getQueueIndex()].fullReflectorInstances.push_back(fullInstanceData);

  // LOD pass
  if (lod.visible)
    switch (lod.level)
    {
    case LOD::Full:
    {
      this->groups[model->getQueueIndex()].fullLODInstances.push_back(fullInstanceData);
      break;
    }

    case LOD::Impostor:
    {
      InstancePositionRadiusTexture data;
      data.position = transform.position;
      data.radius = lod.scaledMeanRadius;
      data.textureLayer = model->getImpostorLayer();

      this->impostors.push_back(data);
      break;
    }

    case LOD::Point:
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
    this->groups[i].fullShadowInstances.insert(this->groups[i].fullShadowInstances.end(), std::make_move_iterator(builder.groups[i].fullShadowInstances.begin()), std::make_move_iterator(builder.groups[i].fullShadowInstances.end()));
    this->groups[i].fullReflectorInstances.insert(this->groups[i].fullReflectorInstances.end(), std::make_move_iterator(builder.groups[i].fullReflectorInstances.begin()), std::make_move_iterator(builder.groups[i].fullReflectorInstances.end()));
  }

  impostors.insert(impostors.end(), std::make_move_iterator(builder.impostors.begin()), std::make_move_iterator(builder.impostors.end()));
  points.insert(points.end(), std::make_move_iterator(builder.points.begin()), std::make_move_iterator(builder.points.end()));
}

void RenderQueueBuilder::finish(InstanceManager &instances, RenderQueue &queue)
{
  for (auto &group : this->groups)
  {
    Range range = instances.add(std::move(group.fullLODInstances));

    if (group.model->getIsTangent())
      queue.addTangentBatch({group.model, range});
    else
      queue.addCoreBatch({group.model, range});
  }

  instances.add(std::move(this->impostors));
  instances.add(std::move(this->points));
}