#include "base/epoll.h"

#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>

// #define EPOLL_DEBUG 1
#ifdef EPOLL_DEBUG

#include <cstdio>

#define PRINTF(fmt, ...) \
  printf("%s:%s:%d" fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define PERROR(fmt) perror(fmt)

#endif  // EPOLL_DEBUG

Epoll::Epoll() {
  if((epoll_fd_ = epoll_create(1)) == -1) {

#ifdef EPOLL_DEBUG
    PRINTF("epoll_create() failed(%d).", errno);
#endif

    exit(-1);
  }
}

Epoll::~Epoll() {
  close(epoll_fd_);
}

// 把fd和它需要监视的事件添加到红黑树上。
void
Epoll::addFd(int fd, uint32_t op) {
  // 声明事件的数据结构。
  epoll_event ev;
  // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
  ev.data.fd = fd;
  // 让epoll监视listenfd的读事件，采用水平触发。
  ev.events = op;

  // 把需要监视的listenfd和它的事件加入epollfd中。
  if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev)) {

#ifdef EPOLL_DEBUG
    PRINTF("epoll_ctl() failed(%d).", errno);
#endif

    exit(-1);
  }

  epoll_event evs[10];  // 存放epoll_wait()返回事件的数组。
}

std::vector<epoll_event>
Epoll::loop(int timeout) {
  // 存放epoll_wait()返回的事件。
  std::vector<epoll_event> evs;

  memset(event_, 0, sizeof(event_));
  // 等待监视的fd有事件发生。
  int infds = epoll_wait(epoll_fd_, event_, max_events, timeout);

  // 返回失败。
  if(infds < 0) {

#ifdef EPOLL_DEBUG
    PRINTF("epoll_wait() failed.");
#endif

    exit(-1);
  }

  // 超时。
  if(infds == 0) {

#ifdef EPOLL_DEBUG
    PRINTF("epoll_wait() timeout.");
#endif

    return evs;
  }

  // 如果infds>0，表示有事件发生的fd的数量。
  // 遍历epoll返回的数组evs。
  for(int i = 0; i < infds; i++) {
    evs.emplace_back(std::move(event_[i]));
  }

  return evs;
}

// void updateChannel(Channel* ch);
// void removeChannel(Channel* ch);