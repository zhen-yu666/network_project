#include "net/acceptor.h"

#include "base/channel.h"
#include "base/socket.h"
#include "net/connection.h"

void
Acceptor::init(const std::string& ip, const uint16_t port) {
  listen_sock_ = std::make_unique<Socket>(createNonbloking());

  InetAddress servaddr(ip, port);
  listen_sock_->setReuseaddr(true);
  listen_sock_->setTcpNodelay(true);
  listen_sock_->setReuseport(true);
  listen_sock_->setKeepalive(true);
  listen_sock_->bind(servaddr);
  listen_sock_->listen();

  listen_channel_ = std::make_unique<Channel>(loop_, listen_sock_->fd());

  // 新连接处理
  listen_channel_->setReadCallback(std::bind(&Acceptor::newConnection, this));
  // 将监听的socket挂在树上
  listen_channel_->enableReading();
}

void
Acceptor::newConnection() {
  // 客户端的地址和协议。
  InetAddress clientaddr;

  std::unique_ptr<Socket> clientsock =
    std::make_unique<Socket>(listen_sock_->accept4(clientaddr, SOCK_NONBLOCK));
  
  clientsock->setIp(clientaddr.ip());
  clientsock->setPort(clientaddr.port());
  // 将新连接的客户端挂在树上。
  // 回调TcpServer::newconnection()
  new_conn_callback_(std::move(clientsock));
}