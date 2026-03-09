#include "net/acceptor.h"

#include "base/channel.h"
#include "base/socket.h"
#include "net/connection.h"

// #define ACCEPTOR_DEBUG 1
#ifdef ACCEPTOR_DEBUG

#include <cstdio>

#define PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__);

#endif

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
  listen_channel_->setCallback(std::bind(&Acceptor::newConnection, this));
  // 让epoll监视监听套接字的事件
  listen_channel_->enableReading();
}

Acceptor::~Acceptor() {
  delete listen_sock_;
  listen_sock_ = nullptr;
  delete listen_channel_;
  listen_channel_ = nullptr;
}

void
Acceptor::newConnection() {
  // 客户端的地址和协议。
  InetAddress clientaddr;
  // 注意，clientsock只能new出来，不能在栈上，否则析构函数会关闭fd。
  // 还有，这里new出来的对象没有释放，这个问题以后再解决。
  Socket* clientsock =
    new Socket(listen_sock_->accept4(clientaddr, SOCK_NONBLOCK));

#ifdef ACCEPTOR_DEBUG
  PRINTF("accept client(fd=%d,ip=%s,port=%d) ok.\n", clientsock->fd(),
         clientaddr.ip(), clientaddr.port());
#endif

  // 为新客户端连接准备读事件，并添加到epoll中。
  // 还有，这里new出来的对象没有释放，这个问题以后再解决。
  Connection* conn = new Connection(loop_, clientsock);
}