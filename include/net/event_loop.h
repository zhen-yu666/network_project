#pragma once

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "base/epoll.h"

#include <functional>

class Epoll;
class Channel;

class EventLoop {
private:
  // 每个事件循环只有一个Epoll。
  Epoll* ep_;
  // epoll_wait()超时的回调函数。
  std::function<void(EventLoop*)> epoll_timeout_callback_;

public:
  // 在构造函数中创建Epoll对象ep_。
  EventLoop() : ep_(new Epoll) {}

  // 在析构函数中销毁ep_。
  ~EventLoop() {
    delete ep_;
    ep_ = nullptr;
  }

  // 运行事件循环。
  void run();

  // 把channel添加/更新到红黑树上
  void updateChannel(Channel* ch);

  // 从红黑树上删除channel。
  void removeChannel(Channel* ch);

  // 设置epoll_wait()超时的回调函数。
  template<typename Callback>
  void setEpollTimeoutCallback(Callback&& cb) {
    epoll_timeout_callback_ = std::forward<Callback>(cb);
  }
};

#endif