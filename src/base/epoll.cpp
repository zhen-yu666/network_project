#include "base/epoll.h"

#include <unistd.h>
#include <cerrno>
#include <cstdlib>

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
Epoll::addFd(int fd, uint32_t op) {}

// 运行epoll_wait()，等待事件的发生，已发生的事件用vector容器返回。
std::vector<epoll_event>
Epoll::loop(int timeout) {}

// void updateChannel(Channel* ch);
// void removeChannel(Channel* ch);