#pragma once

template <typename F>
void ThreadPool::enqueue(F &&task)
{
  {
    std::lock_guard<std::mutex> lock(mtx);
    tasks.emplace(std::forward<F>(task));
  }
  cv.notify_one();
};