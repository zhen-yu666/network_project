#include "base/channel.h"

#include "base/epoll.h"
#include "net/event_loop.h"

#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

// #define CHANNEL_DEBUG 1
#ifdef CHANNEL_DEBUG

#include <cstdio>

#define PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__);

#endif

void
Channel::enableReading() {
  events_ |= EPOLLIN;
  loop_->updateChannel(this);
}

void
Channel::disableReading() {
  events_ &= ~EPOLLIN;
  loop_->updateChannel(this);
}

void
Channel::enableWriting() {
  events_ |= EPOLLOUT;
  loop_->updateChannel(this);
}

void
Channel::disableWriting() {
  events_ &= ~EPOLLOUT;
  loop_->updateChannel(this);
}

void
Channel::handLevent() {
  if(revents_ & EPOLLRDHUP) {
    // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
    closeCallback_();
  } else if(revents_ & (EPOLLIN | EPOLLPRI)) {
    // 如果有新连接，设置回调函数为newConnection
    // 如果有数据到达，设置回调函数为onMessage
    readCallback_();
  } else if(revents_ & EPOLLOUT) {
    // 有数据需要写，暂时没有代码，以后再说。
    writeCallback_();
  } else {
    // 其它事件，都视为错误。
    errorCallback_();
  }
}