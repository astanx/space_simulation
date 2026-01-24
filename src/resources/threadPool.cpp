#include "resources/threadPool.h"


// Constructor/Destructor
ThreadPool::ThreadPool(size_t count) {
  if (count == 0)
    count = 1;
  this->workers.reserve(count);
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(this->mtx);
    this->stop = true;
  }
  this->cv.notify_all();
  for (std::thread &worker : this->workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

// Public functions
void ThreadPool::wait() {
  std::unique_lock<std::mutex> lock(this->mtx);
  this->cv.wait(lock, [this]() { return this->tasks.empty(); });
}

std::mutex& ThreadPool::getMutex() {
  return this->mtx;
}