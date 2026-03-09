#include "net/connection.h"

#include "base/channel.h"

#include <unistd.h>

void
Connection::init() {
  // 将新连接挂在树上中。
  client_channel_ = new Channel(loop_, client_sock_->fd());
  client_channel_->setReadCallback(
    std::bind(&Channel::onMessage, client_channel_));
  client_channel_->setCloseCallback(
    std::bind(&Connection::closeCallback, this));
  client_channel_->setErrorCallback(
    std::bind(&Connection::errorCallback, this));
  // 客户端连上来的fd采用边缘触发。
  client_channel_->useET();
  // 让epoll监视clientchannel的读事件。
  client_channel_->enableReading();
}

Connection::~Connection() {
  delete client_sock_;
  client_sock_ = nullptr;
  delete client_channel_;
  client_channel_ = nullptr;
}