#include "resources/threadPool.h"

#include <iostream>

// Constructor/Destructor
ThreadPool::ThreadPool(size_t threads)
{
  if (threads == 0)
  {
    threads = std::thread::hardware_concurrency();
    std::cout << "Threads supported: " << threads << std::endl;
    if (threads == 0)
      threads = 4;
  }
  this->workers.reserve(threads);

  for (size_t i = 0; i < threads; ++i)
  {
    workers.emplace_back([this]
                         {
        while (true) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(this->mtx);
          this->cv.wait(lock, [this] {
            return this->stop || !this->tasks.empty();
          });

          if (this->stop && this->tasks.empty()) {
            return;
          }

          task = std::move(this->tasks.front());
          this->tasks.pop();
          ++this->busy_threads;
        }
                

        task();               
        {
          std::unique_lock<std::mutex> lock(this->mtx);
          --this->busy_threads;   
        }

        this->cv.notify_all(); 
      } });
  }
}

ThreadPool::~ThreadPool()
{
  {
    std::unique_lock<std::mutex> lock(this->mtx);
    this->stop = true;
  }
  this->cv.notify_all();
  for (std::thread &worker : this->workers)
  {
    if (worker.joinable())
    {
      worker.join();
    }
  }
}

// Public functions
void ThreadPool::wait()
{
  std::unique_lock<std::mutex> lock(this->mtx);
  this->cv.wait(lock, [this]()
                { return stop || (tasks.empty() && busy_threads == 0); });
}

std::mutex &ThreadPool::getMutex()
{
  return this->mtx;
}