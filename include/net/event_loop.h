#pragma once

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "base/epoll.h"

#include <functional>
#include <mutex>
#include <thread>

class Epoll;
class Channel;

class EventLoop {
private:
  // 每个事件循环只有一个Epoll。
  Epoll* ep_;
  // epoll_wait()超时的回调函数。
  std::function<void(EventLoop*)> epoll_timeout_callback_;

  // 记录本 EventLoop 所属的线程 ID
  std::thread::id thread_id_;
  // eventfd 文件描述符，用于唤醒
  int wakeup_fd_;
  // 包装 wakeupFd_ 的 Channel
  std::unique_ptr<Channel> wakeup_channel_;
  // 待执行的任务队列（由其他线程投递）
  std::vector<std::function<void()>> pending_functors_;
  // 保护 pendingFunctors_ 的互斥锁
  std::mutex mtx_;
  // 标记当前是否正在执行 pending functors
  bool calling_pending_functors_;

private:
  void init();

  // 向 wakeupFd_ 写入数据，唤醒阻塞在 epoll_wait 的 loop 线程
  void wakeup();

  // wakeupChannel_ 的读回调，用于读取 eventfd 数据，清空通知
  void handleRead();

  // 执行所有待处理的 pending functors
  void doPendingFunctors();

public:
  // 在构造函数中创建Epoll对象ep_。
  EventLoop()
      : ep_(new Epoll),
        thread_id_(std::this_thread::get_id()),
        calling_pending_functors_(false) {
    init();
  }

  ~EventLoop();

  // 运行事件循环。
  void run();

  // 更新/添加 Channel 到 epoll
  void updateChannel(Channel* ch);

  // 从 epoll 移除 Channel
  void removeChannel(Channel* ch);

  // 设置epoll_wait()超时的回调函数。
  template<typename Callback>
  void setEpollTimeoutCallback(Callback&& cb) {
    epoll_timeout_callback_ = std::forward<Callback>(cb);
  }

  // 判断当前线程是否是本 loop 线程
  bool isInLoopThread() const {
    return std::this_thread::get_id() == thread_id_;
  }

  // 如果当前是 loop 线程则直接执行 cb，否则将 cb 放入队列并唤醒 loop 线程
  void runInLoop(std::function<void()> cb);

  // 无论当前线程是谁，都将 cb 放入队列并唤醒 loop 线程（如果需要）
  void queueInLoop(std::function<void()> cb);
};

#endif