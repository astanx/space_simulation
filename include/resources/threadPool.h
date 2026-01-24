#pragma once

#include <vector>
#include <thread>

class ThreadPool
{
private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex mtx;
  std::condition_variable cv;
  bool stop = false;

public:
  ThreadPool(size_t count);
  ~ThreadPool();

  inline unsigned getThreadCount() const { return workers.size(); }

  template <class F>
  void enqueue(F &&task)
  {
    {
      std::unique_lock<std::mutex> lock(this->mtx);
      this->tasks.emplace(std::forward<F>(task));
    }
    this->cv.notify_one();
  }

  void wait();
  std::mutex &getMutex();
};
