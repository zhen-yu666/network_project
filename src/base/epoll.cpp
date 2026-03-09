#include "base/channel.h"
#include "base/epoll.h"

#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>

// #define EPOLL_DEBUG 1
#ifdef EPOLL_DEBUG

#pragma message("EPOLL_DEBUG is defined")

#include <cstdio>

#define PRINTF(fmt, ...) \
  printf("%s:%s:%d" fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define PERROR(fmt) perror(fmt)

#endif

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

void
Epoll::updateChannel(Channel* ch) {
  epoll_event ev;
  ev.data.ptr = ch;
  ev.events = ch->getEvents();

  if(ch->getInEpoll()) {
    // 当前socket在epoll的红黑树上。
    if(epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, ch->getFd(), &ev) == -1) {

#ifdef EPOLL_DEBUG
      PERROR("epoll_ctl() failed.");
#endif

      exit(-1);
    }
  } else {
    // 当前socket不在epoll的红黑树上。
    // 将socket挂载树上，并在对应socket注册回调函数
    if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, ch->getFd(), &ev) == -1) {

#ifdef EPOLL_DEBUG
      PERROR("epoll_ctl() failed.");
#endif

      exit(-1);
    }
    // 设置在树上的标记，不用向内核询问。
    ch->setInEpoll();
  }
}

std::vector<Channel*>
Epoll::loop(int timeout) {
  // 每一个对应的事件所带信息的集合。
  std::vector<Channel*> channels;

  memset(events_, 0, sizeof(events_));
  // 将rdlist的内容拷贝到events_，然后加入到对应的channel中。
  int infds = epoll_wait(epoll_fd_, events_, max_events, timeout);

  // 返回失败。
  if(infds < 0) {

#ifdef EPOLL_DEBUG
    PERROR("epoll_wait() failed");
#endif

    exit(-1);
  }

  // 超时。
  if(infds == 0) {

#ifdef EPOLL_DEBUG
    PERROR("epoll_wait() timeout");
#endif

    return channels;
  }

  // 如果infds>0，表示有事件发生的fd的数量。
  // 遍历epoll返回的数组events_。
  for(int i = 0; i < infds; ++i) {
    // 取出已发生事件的信息合集
    Channel* ch = reinterpret_cast<Channel*>(events_[i].data.ptr);
    // 设置将对应的信息加入集合
    ch->setRevents(events_[i].events);
    channels.emplace_back(std::move(ch));
  }

  return channels;
}