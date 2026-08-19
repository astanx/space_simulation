#pragma once

// Public functions
template <std::floating_point Real>
void Scene::init(RenderContext &renderCtx, ResourceManager &resourceManager, ThreadPool &threadPool, double startTime)
{
  this->world.emplace<SimulationWorld<Real>>();
  std::visit([&renderCtx, &resourceManager, &threadPool, startTime](auto &w)
             { w.init(renderCtx, resourceManager, threadPool, startTime); }, this->world);
}