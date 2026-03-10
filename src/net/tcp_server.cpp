#include "net/tcp_server.h"

#include "base/socket.h"
#include "net/connection.h"

#include <unistd.h>
#include <cstring>

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
  loop_.setEpollTimeoutCallback(
    std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));
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
  conn->setOnMsgCallback(std::bind(
    &TcpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
  conn->setSendCompleteCallback(
    std::bind(&TcpServer::sendComplete, this, std::placeholders::_1));

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

void
TcpServer::onMessage(Connection* conn, const std::string& msg) {
  // 构造响应数据
  std::string reply = "reply:" + msg;
  // 计算回应报文的大小
  int len = reply.size();
  // 把报文头部填充到回应报文中
  std::string tmp_buf((char*)&len, 4);
  // 把报文内容填充到回应报文中
  tmp_buf.append(reply);

  // 把临时缓冲区中的数据发送出去
  conn->send(tmp_buf.data(), tmp_buf.size());
}

void
TcpServer::sendComplete(Connection* conn) {
  printf("send complete.\n");
  // 业务代码
}

void
TcpServer::epollTimeout(EventLoop* loop) {
  printf("epoll_wait() timeout.\n");
  // 业务代码
}