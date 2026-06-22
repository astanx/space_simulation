#include "resources/threadPool.h"

#include "resources/range.h"

#include "debug/logger.h"

#include <iostream>

// Constructor/Destructor
ThreadPool::ThreadPool(size_t threads)
{
  if (threads == 0)
  {
    threads = std::thread::hardware_concurrency();
    Logger::logInfo("Thread pool", "Supported " + std::to_string(threads) + " threads");
    if (threads == 0)
      Logger::logFatal("Thread pool", "0 threads supported");
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
      worker.join();
  }
}

// Public functions
void ThreadPool::wait()
{
  std::unique_lock<std::mutex> lock(this->mtx);
  this->cv.wait(lock, [this]()
                { return stop || (tasks.empty() && busy_threads == 0); });
}

void ThreadPool::enqueue(std::function<void()> &&task)
{
  {
    std::unique_lock<std::mutex> lock(this->mtx);
    this->tasks.emplace(std::move(task));
  }
  this->cv.notify_one();
}

std::mutex &ThreadPool::getMutex()
{
  return this->mtx;
}

void ThreadPool::initRanges(std::vector<Range> &ranges, size_t total)
{
  size_t threadCount = this->getThreadCount();
  ranges.resize(threadCount);
  unsigned int perThread = total / threadCount;
  unsigned int remaining = total % threadCount;
  size_t start = 0;
  for (size_t i = 0; i < threadCount; i++)
  {
    unsigned work = perThread + (i < remaining ? 1 : 0);

    unsigned begin = start;
    unsigned end = begin + work;

    ranges[i] = {begin, end};
    start = end;
  }
}