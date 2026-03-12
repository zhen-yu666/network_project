#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "base/thread_pool.h"
#include "net/acceptor.h"
#include "net/connection.h"
#include "net/event_loop.h"

#include <string>
#include <unordered_map>

class Acceptor;

class TcpServer {
private:
  // 主事件循环。
  EventLoop* main_loop_ = nullptr;
  // 存放从事件循环的容器。
  std::vector<std::unique_ptr<EventLoop>> sub_loops_;
  // 线程池
  std::unique_ptr<ThreadPool> thread_pool_;
  // 一个服务器只有一个监听对象。
  std::unique_ptr<Acceptor> acceptor_;
  // 一个TcpServer有多个Connection对象，存放在map容器中。
  std::unordered_map<int, SptrConnection> conns_;
  // 用于互斥访问 conns_
  std::mutex mtx_;

  // 回调EchoServer::HandleNewConnection()。
  std::function<void(SptrConnection)> new_conn_callback_;
  // 回调EchoServer::HandleClose()。
  std::function<void(SptrConnection)> close_conn_callback_;
  // 回调EchoServer::HandleError()。
  std::function<void(SptrConnection)> error_conn_callback_;
  // 回调EchoServer::HandleMessage()。
  std::function<void(SptrConnection, const std::string& message)>
    on_msg_callback_;
  // 回调EchoServer::HandleSendComplete()。
  std::function<void(SptrConnection)> send_complete_callback_;
  // 回调EchoServer::HandleTimeOut()。
  std::function<void(EventLoop*)> timeout_callback_;

  // 线程池的大小，即从事件循环的个数。
  int thread_num_;

private:
  void init(const std::string& ip, const uint16_t port);

public:
  TcpServer(const std::string& ip, const uint16_t port, int thread_num)
      : thread_num_(thread_num) {
    init(ip, port);
  }

  ~TcpServer();

  // 运行每一个所管理的事件循环。
  void start();

  // 处理新客户端连接请求。
  void newConnection(std::unique_ptr<Socket> client_sock);

  // 关闭客户端的连接。
  void closeConnection(SptrConnection conn);

  // 客户端的连接错误。
  void errorConnection(SptrConnection conn);

  // 处理对端发送到服务端的信息。
  void onMessage(SptrConnection conn, const std::string& msg);

  // 数据发送完成后，在Connection类中回调此函数。
  void sendComplete(SptrConnection conn);

  // epoll_wait()超时，
  void epollTimeout(EventLoop* loop);

  void removeConnection(SptrConnection conn);

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