#include "net/event_loop.h"
#include "base/channel.h"

void
EventLoop::run() {
  while(true) {
    // 存储epoll的rdlist的所有事件
    std::vector<Channel*> channels = std::move(ep_->loop());

    // 处理每个事件
    for(auto& ch : channels) {
      ch->handLevent();
    }
  }
}

void
EventLoop::updateChannel(Channel* ch) {
  ep_->updateChannel(ch);
}