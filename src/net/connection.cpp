#include "net/connection.h"
#include "base/channel.h"
#include "base/socket.h"

void
Connection::init() {
  // 为新客户端连接准备读事件，并添加到epoll中。
  client_channel_ = new Channel(loop_, client_sock_->fd());
  client_channel_->setCallback(std::bind(&Channel::onMessage, client_channel_));
  // 客户端连上来的fd采用边缘触发。
  client_channel_->useET();
  // 让epoll监视clientchannel的读事件。
  client_channel_->enableReading();
}

Connection::~Connection() {}
