#pragma once

#include "resources/range.h"

// Public functions
template <typename F>
void ThreadPool::parallelFor(size_t start, size_t end, F &&task)
{
  size_t total = end - start;

  if (total < this->threshold)
  {
    Range range{start, end};
    if constexpr (std::is_invocable_v<std::decay_t<F>, Range, size_t>)
      task(range, 0);
    else if constexpr (std::is_invocable_v<std::decay_t<F>, Range>)
      task(range);
    else if constexpr (std::is_invocable_v<std::decay_t<F>, size_t>)
      for (size_t i = range.begin; i < range.end; i++)
        task(i);
  }
  else
  {
    std::vector<Range> ranges;
    this->calculateRanges(ranges, total);

    for (size_t thread = 0; thread < this->getThreadCount(); thread++)
    {
      Range range = ranges[thread];
      if constexpr (std::is_invocable_v<std::decay_t<F>, Range, size_t>)
        this->enqueue([range, &task, thread]()
                      { task(range, thread); });
      else if constexpr (std::is_invocable_v<std::decay_t<F>, Range>)
        this->enqueue([range, &task]()
                      { task(range); });
      else if constexpr (std::is_invocable_v<std::decay_t<F>, size_t>)
        this->enqueue([range, &task]()
                      { 
                      for(size_t i = range.begin; i < range.end; i++)
                        task(i); });
    }
    this->wait();
  }
}

template <typename F>
void ThreadPool::enqueue(F &&task)
{
  {
    std::lock_guard<std::mutex> lock(mtx);
    tasks.emplace(std::forward<F>(task));
  }
  cv.notify_one();
};