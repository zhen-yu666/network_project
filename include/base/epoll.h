#pragma once

#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include <vector>

class Channel;

class Epoll {
private:
  // epoll_wait()返回事件数组的大小。
  constexpr static int max_events = 100;
  // epoll句柄，在构造函数中创建。
  int epoll_fd_ = -1;
  // 存放poll_wait()返回事件的数组，在构造函数中分配内存。
  epoll_event events_[100];

public:
  Epoll();

  ~Epoll();

  // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
  void updateChannel(Channel* ch);

  // 从红黑树上删除channel。
  void removeChannel(Channel* ch);

  // 运行epoll_wait()，等待事件的发生，已发生的事件用vector容器返回。
  std::vector<Channel*> loop(int timeout = -1);
};

#endif  // EPOLL_H