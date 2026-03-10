#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "net/acceptor.h"
#include "net/event_loop.h"
#include "net/connection.h"

#include <string>
#include <unordered_map>

class Acceptor;

class TcpServer {
private:
  // 一个TcpServer可以有多个事件循环。
  EventLoop loop_;
  // 一个服务器只有一个监听对象。
  Acceptor* acceptor_ = nullptr;
  // 一个服务器有多个已连接对象。
  std::unordered_map<int, Connection*> conns_;

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

  // 关闭客户端的连接。
  void closeConnection(Connection* conn);
  
  // 客户端的连接错误。
  void errorConnection(Connection* conn);

  // 处理对端发送到服务端的信息。
  void onMessage(Connection* conn, const std::string& msg);
};

#endif