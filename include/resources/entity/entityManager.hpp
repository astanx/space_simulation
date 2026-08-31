#pragma once

template <typename F>
void EntityManager::forEachSpecialEntity(F &&func) const
{
  for (const auto &[entity, idx] : this->entityToSpecialIdx)
    func(entity);
}
