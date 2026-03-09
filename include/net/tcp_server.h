#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "net/event_loop.h"
#include "net/acceptor.h"

#include <string>

class Acceptor;

class TcpServer {
private:
  // 一个TcpServer可以有多个事件循环。
  EventLoop loop_;
  // 一个服务器只有一个监听套接字
  Acceptor* acceptor_;

private:
  void init(const std::string& ip, const uint16_t port);

public:
  TcpServer(const std::string& ip, const uint16_t port)
      : acceptor_(new Acceptor(&loop_, ip, port)) {}

  ~TcpServer();

  // 运行每一个所管理的事件循环。
  void start();
};

#endif