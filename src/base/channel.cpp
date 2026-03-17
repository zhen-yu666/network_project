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
Channel::disableAll() {
  events_ = 0;
  loop_->updateChannel(this);
}

void
Channel::removeNode() {
  disableAll();
  loop_->removeChannel(this);
}

#include <cstdio>

void
Channel::handLevent() {
  if(revents_ & EPOLLRDHUP) {
    // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
    close_callback_();
  } else if(revents_ & (EPOLLIN | EPOLLPRI)) {
    // 如果是acceptchannel，将回调Acceptor::newconnection()
    // 如果是clientchannel，将回调Connection::onmessage()。
    read_callback_();
  } else if(revents_ & EPOLLOUT) {
    // 有数据需要向外发送
    // 回调Connection::writecallback()。
    write_callback_();
  } else {
    // 其它事件，都视为错误。
    error_callback_();
  }
}