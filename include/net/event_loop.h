#pragma once

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "base/epoll.h"

class Epoll;
class Channel;

class EventLoop {
private:
  // 每个事件循环只有一个Epoll。
  Epoll* ep_;

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

  // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
  // 一个ep_对应多个Channel，等价于一个事件循环对应多个Channel。
  void updateChannel(Channel* ch);
};

#endif