#pragma once

#include <vector>
#include <thread>

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

public:
  ThreadPool(size_t threads = 0);
  ~ThreadPool();

  inline unsigned getThreadCount() const { return workers.size(); }

  void enqueue(std::function<void()> &&task);

  void wait();
  std::mutex &getMutex();

  void initRanges(std::vector<Range> &ranges, size_t total);
};
