#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "net/acceptor.h"
#include "net/connection.h"
#include "net/event_loop.h"

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
  std::function<void(Connection*)> new_conn_callback_;
  std::function<void(Connection*)> close_conn_callback_;
  std::function<void(Connection*)> error_conn_callback_;
  std::function<void(Connection*, const std::string& message)> on_msg_callback_;
  std::function<void(Connection*)> send_complete_callback_;
  std::function<void(EventLoop*)> timeout_callback_;

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

  // 数据发送完成后，在Connection类中回调此函数。
  void sendComplete(Connection* conn);

  // epoll_wait()超时，
  void epollTimeout(EventLoop* loop);

  template<typename Callback>
  void setNewConnCallback(Callback&& cb) {
    new_conn_callback_ = std::forward<Callback>(cb);
  }

  template<typename Callback>
  void setCloseConnCallback(Callback&& cb) {
    close_conn_callback_ = std::forward<Callback>(cb);
  }

  template<typename Callback>
  void setErrorConnCallback(Callback&& cb) {
    error_conn_callback_ = std::forward<Callback>(cb);
  }

  template<typename Callback>
  void setOnMsgCallback(Callback&& cb) {
    on_msg_callback_ = std::forward<Callback>(cb);
  }

  template<typename Callback>
  void setSendCompleteCallback(Callback&& cb) {
    send_complete_callback_ = std::forward<Callback>(cb);
  }

  template<typename Callback>
  void setTimeoutCallback(Callback&& cb) {
    timeout_callback_ = std::forward<Callback>(cb);
  }
};

#endif