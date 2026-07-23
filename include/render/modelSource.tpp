#pragma once

#include "render/renderFlags.h"

#include "graphics/model.h"

template <typename F>
void ModelSource::forEachModel(F &&func)
{
  if constexpr (std::is_invocable_v<std::decay_t<F>, Model &, RenderFlags>)
  {
    func(*this->mainLayer, RenderFlags::Main);

    for (auto &layer : this->layers)
      func(*layer, RenderFlags::Layer);
  }
  else if constexpr (std::is_invocable_v<std::decay_t<F>, Model &>)
  {
    func(*this->mainLayer);

    for (auto &layer : this->layers)
      func(*layer);
  }
}