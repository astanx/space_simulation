#pragma once

template <typename F>
void EntityManager::forEachSpecialEntity(F &&func) const
{
  for (const auto [entityID, idx] : this->entityToSpecialIdx)
    func(Entity{entityID});
}
