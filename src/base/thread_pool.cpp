#include "base/thread_pool.h"

#include <sys/syscall.h>
#include <unistd.h>

#include <cstdio>

void
ThreadPool::init(size_t thread_num) {
  // 启动threadnum个线程，每个线程将阻塞在条件变量上。
  for(size_t i = 0; i < thread_num; i++) {
    // 创建线程
    threads_.emplace_back([this] {
      while(!stop_) {
        // 用于存放出队的元素。
        std::function<void()> task;

        {
          std::unique_lock<std::mutex> lock(mtx_);

          // 等待生产者的条件变量。
          this->cv_.wait(lock,
                         [this] { return stop_ || !task_queue_.empty(); });

          // 在线程池停止之前，队列无任务，直接退出
          if(stop_ && task_queue_.empty())
            return;

          // 出队一个任务。
          task = std::move(task_queue_.front());
          task_queue_.pop();
        }

        // 执行任务。
        task();
      }
    });
  }
}

void
ThreadPool::destory() {
  stop_ = true;
  // 唤醒全部的线程
  cv_.notify_all();
  // 等待全部线程退出
  for(std::thread& th : threads_)
    th.join();
}
