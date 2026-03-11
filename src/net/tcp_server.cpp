#include "net/tcp_server.h"

#include "base/socket.h"
#include "net/connection.h"

#include <unistd.h>
#include <cstring>

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

  conns_.insert({conn->fd(), conn});

  if(new_conn_callback_)
    new_conn_callback_(conn);
}

void
TcpServer::closeConnection(Connection* conn) {
  if(close_conn_callback_)
    close_conn_callback_(conn);

  conns_.erase(conn->fd());
  // 调用Connection的析构，RAII
  delete conn;
  conn = nullptr;
}

void
TcpServer::errorConnection(Connection* conn) {
  if(error_conn_callback_)
    error_conn_callback_(conn);

  conns_.erase(conn->fd());
  // 调用Connection的析构，RAII
  delete conn;
  conn = nullptr;
}

void
TcpServer::onMessage(Connection* conn, const std::string& msg) {
  if(on_msg_callback_)
    on_msg_callback_(conn, msg);
}

void
TcpServer::sendComplete(Connection* conn) {
  if(send_complete_callback_)
    send_complete_callback_(conn);
}

void
TcpServer::epollTimeout(EventLoop* loop) {
  if(timeout_callback_)
    timeout_callback_(loop);
}