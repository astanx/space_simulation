#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

struct Range;

class ThreadPool
{
private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex mtx;
  std::condition_variable cv;
  bool stop = false;
  std::atomic<unsigned> busy_threads{0};

  size_t threshold = 1000;

public:
  ThreadPool(size_t threads = 0);
  ~ThreadPool();

  inline unsigned getThreadCount() const { return workers.size(); }

  template <typename F>
  void parallelFor(size_t start, size_t end, F &&task);

  template <typename F>
  void enqueue(F &&task);

  void wait();
  std::mutex &getMutex();

  void calculateRanges(std::vector<Range> &ranges, size_t total);
};

#include "resources/threadPool.tpp"