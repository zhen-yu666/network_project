#pragma once

#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H

#include "net/tcp_server.h"

#include <string>

class Connection;
class EventLoop;

class EchoServer {
private:
  TcpServer tcpserver_;

private:
  void init();

public:
  EchoServer(const std::string& ip, const uint16_t port)
      : tcpserver_(ip, port) {
    init();
  }

  ~EchoServer() = default;

  // 启动服务。
  void Start();

  // 处理新客户端连接请求
  void handleNewConnection(Connection* conn);

  // 关闭客户端的连接
  void handleClose(Connection* conn);

  // 客户端的连接错误
  void handleError(Connection* conn);

  // 处理客户端的请求报文
  void handleMessage(Connection* conn, const std::string& message);

  // 数据发送完成后
  void handleSendComplete(Connection* conn);

  // epoll_wait()超时
  void handleTimeout(EventLoop* loop);
};

#endif