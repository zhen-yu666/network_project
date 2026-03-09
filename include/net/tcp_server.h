#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "net/acceptor.h"
#include "net/event_loop.h"

#include <string>

class Acceptor;

class TcpServer {
private:
  // 一个TcpServer可以有多个事件循环。
  EventLoop loop_;
  // 一个服务器只有一个监听套接字
  Acceptor* acceptor_ = nullptr;

private:
  void init(const std::string& ip, const uint16_t port);

public:
  template<typename Ip, typename Port>
  TcpServer(Ip&& ip, Port&& port) {
    init(std::forward<Ip>(ip), std::forward<Port>(port));
  }

  ~TcpServer();

  // 运行每一个所管理的事件循环。
  void start();

  // 处理新客户端连接请求。
  void newConnection(Socket* client_sock);
};

#endif