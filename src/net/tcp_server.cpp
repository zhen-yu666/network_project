#include "net/tcp_server.h"

#include "base/socket.h"
#include "net/connection.h"

// #define TCP_SERVER_DEBUG 1
#ifdef TCP_SERVER_DEBUG
#include <cstdio>
#define PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__);
#endif

void
TcpServer::init(const std::string& ip, const uint16_t port) {
  acceptor_ = new Acceptor(&loop_, ip, port);
  acceptor_->setNewConnCallback(
    std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer() {
  delete acceptor_;
  acceptor_ = nullptr;
}

void
TcpServer::start() {
  loop_.run();
}

void
TcpServer::newConnection(Socket* client_sock) {

#ifdef TCP_SERVER_DEBUG
  PRINTF("accept client(fd=%d,ip=%s,port=%d) ok.\n", client_sock->fd(),
         client_sock->ip().c_str(), client_sock->port());
#endif

  // 还有，这里new出来的对象没有释放，这个问题以后再解决。
  Connection* conn = new Connection(&loop_, client_sock);
}