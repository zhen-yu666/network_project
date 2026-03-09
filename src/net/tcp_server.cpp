#include "net/tcp_server.h"

TcpServer::~TcpServer() {
  delete acceptor_;
  acceptor_ = nullptr;
}

void
TcpServer::start() {
  loop_.run();
}