#include <unistd.h>
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
  for(auto& conn : conns_) {
    delete conn.second;
    conn.second = nullptr;
  }
}

void
TcpServer::start() {
  loop_.run();
}

void
TcpServer::newConnection(Socket* client_sock) {
  Connection* conn = new Connection(&loop_, client_sock);
  conn->setCloseCallback(
    std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
  conn->setErrorCallback(
    std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));

#ifdef TCP_SERVER_DEBUG
  PRINTF("new connection(fd=%d,ip=%s,port=%d) ok.\n", conn->fd(),
         conn->ip().c_str(), conn->port());
#endif

  conns_.insert({conn->fd(), conn});
}

void
TcpServer::closeConnection(Connection* conn) {

#ifdef TCP_SERVER_DEBUG
  PRINTF("client(eventfd=%d) disconnected.\n", conn->fd());
#endif
  conns_.erase(conn->fd());
  // 调用Connection的析构，RAII
  delete conn;
  conn = nullptr;
}

void
TcpServer::errorConnection(Connection* conn) {

#ifdef TCP_SERVER_DEBUG
  PRINTF("client(eventfd=%d) error.\n", conn->fd());
#endif
  conns_.erase(conn->fd());
  // 调用Connection的析构，RAII
  delete conn;
  conn = nullptr;
}