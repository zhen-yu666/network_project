#include "base/channel.h"
#include "base/epoll.h"

void
Channel::enableReading() {
  events_ |= EPOLLIN;
  ep_->updateChannel(this);
}