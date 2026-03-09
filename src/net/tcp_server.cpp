#include "base/socket.h"
#include "net/tcp_server.h"
#include "base/channel.h"

void
TcpServer::init(const std::string& ip, const uint16_t port) {
  Socket* serv_sock = new Socket((createNonbloking()));
  // 服务端的地址和协议。
  InetAddress servaddr(ip, port);
  serv_sock->setReuseaddr(true);
  serv_sock->setTcpNodelay(true);
  serv_sock->setReuseport(true);
  serv_sock->setKeepalive(true);
  serv_sock->bind(servaddr);
  serv_sock->listen();

  // 这里new出来的对象没有释放，这个问题以后再解决。
  Channel* serv_channel = new Channel(&loop_, serv_sock->fd());
  // 新连接处理
  serv_channel->setCallback(
    std::bind(&Channel::newConnection, serv_channel, serv_sock));
  // 让epoll_wait()监视servchannel的读事件。
  serv_channel->enableReading();
}

TcpServer::~TcpServer() {}

void
TcpServer::start() {
  loop_.run();
}