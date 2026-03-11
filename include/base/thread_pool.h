#pragma once

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
private:
  // 线程池中的线程。
  std::vector<std::thread> threads_;
  // 任务队列。
  std::queue<std::function<void()>> task_queue_;
  // 任务队列同步的互斥锁。
  std::mutex mtx_;
  // 任务队列同步的条件变量。
  std::condition_variable cv_;
  // 在析构函数中，把stop_的值设置为true，全部的线程将退出。
  std::atomic_bool stop_ = false;

private:
  void init(size_t thread_num);

  void destory();

public:
  ThreadPool(size_t thread_num) { init(thread_num); }

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  ~ThreadPool() { destory(); }

  // 把任务添加到队列中。
  template<typename Task>
  void addTask(Task&& task) {
    {
      std::lock_guard<std::mutex> lock(mtx_);
      task_queue_.push(std::forward<Task>(task));
    }
    // 唤醒一个线程。
    cv_.notify_one();
  }
};

#endif