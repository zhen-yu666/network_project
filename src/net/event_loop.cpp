#include "base/channel.h"
#include "net/event_loop.h"

void
EventLoop::run() {
  while(true) {
    // 存储epoll的rdlist的所有事件
    std::vector<Channel*> channels = std::move(ep_->loop(10 * 1000));

    if(channels.size() == 0) {
      epoll_timeout_callback_(this);
    } else {
      // 处理每个事件
      for(auto& ch : channels) {
        ch->handLevent();
      }
    }
  }
}

void
EventLoop::updateChannel(Channel* ch) {
  ep_->updateChannel(ch);
}

void
EventLoop::removeChannel(Channel* ch) {
  ep_->removeChannel(ch);
}