#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "net/event_loop.h"

#include <string>

class TcpServer {
private:
  // 一个TcpServer可以有多个事件循环。
  EventLoop loop_;

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
};

#endif