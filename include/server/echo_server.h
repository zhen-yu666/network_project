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
  std::unique_ptr<ThreadPool> threads_;

private:
  void init();

public:
  EchoServer(const std::string& ip, const uint16_t port, int sub_thread_num = 2,
             int work_thread_num = 2)
      : tcpserver_(ip, port, sub_thread_num),
        threads_(std::make_unique<ThreadPool>(work_thread_num)) {
    init();
  }

  ~EchoServer() = default;

  // 启动服务。
  void start();

  // 停止服务
  void stop();

  // 处理新客户端连接请求
  void handleNewConnection(SptrConnection conn);

  // 关闭客户端的连接
  void handleClose(SptrConnection conn);

  // 客户端的连接错误
  void handleError(SptrConnection conn);

  // 处理客户端的请求报文
  void handleMessage(SptrConnection conn, const std::string& message);

  // 数据发送完成后
  void handleSendComplete(SptrConnection conn);

  // epoll_wait()超时
  void handleTimeout(EventLoop* loop);

  // 处理客户端的请求报文，用于添加给线程池。
  void onMessage(SptrConnection conn, const std::string& message);
};

#endif