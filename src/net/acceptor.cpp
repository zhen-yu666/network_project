#include "base/channel.h"
#include "base/socket.h"
#include "net/acceptor.h"

void
Acceptor::init(const std::string& ip, const uint16_t port) {
  listen_sock_ = new Socket((createNonbloking()));
  InetAddress servaddr(ip, port);
  listen_sock_->setReuseaddr(true);
  listen_sock_->setTcpNodelay(true);
  listen_sock_->setReuseport(true);
  listen_sock_->setKeepalive(true);
  listen_sock_->bind(servaddr);
  listen_sock_->listen();

  listen_channel_ = new Channel(loop_, listen_sock_->fd());
  // 新连接处理
  listen_channel_->setCallback(
    std::bind(&Channel::newConnection, listen_channel_, listen_sock_));
  // 让epoll监视监听套接字的事件
  listen_channel_->enableReading();
}

Acceptor::~Acceptor() {
  delete listen_sock_;
  listen_sock_ = nullptr;
  delete listen_channel_;
  listen_channel_ = nullptr;
}