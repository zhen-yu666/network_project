#pragma once

#ifndef CONNECTION_H
#define CONNECTION_H

#include "base/socket.h"

#include <functional>

class EventLoop;
class Socket;
class Channel;

class Connection {
private:
  // Connection对应的事件循环，在构造函数中传入。
  EventLoop* loop_ = nullptr;
  // 与客户端通讯的Socket。
  Socket* client_sock_ = nullptr;
  // Connection对应的channel，在构造函数中创建。
  Channel* client_channel_ = nullptr;
  // 设置关闭fd_的回调函数。
  std::function<void(Connection*)> closeCallback_;
  // 设置fd_发生了错误的回调函数。
  std::function<void(Connection*)> errorCallback_;

private:
  void init();

public:
  Connection(EventLoop* loop, Socket* client_sock_)
      : loop_(loop), client_sock_(client_sock_) {
    init();
  }

  ~Connection();

  // 返回fd_成员。
  const int fd() const { return client_sock_->fd(); }

  // 返回ip_成员。
  const std::string& ip() const { return client_sock_->ip(); }

  // 返回port_成员。
  uint16_t port() const { return client_sock_->port(); }

  // TCP连接关闭（断开）的回调函数。
  void closeCallback() { closeCallback_(this); }

  // TCP连接错误的回调函数。
  void errorCallback() { errorCallback_(this); }

  // 设置关闭fd_的回调函数。
  template<typename Callback>
  void setCloseCallback(Callback&& cb) {
    closeCallback_ = std::forward<Callback>(cb);
  }

  // 设置fd_发生了错误的回调函数。
  template<typename Callback>
  void setErrorCallback(Callback&& cb) {
    errorCallback_ = std::forward<Callback>(cb);
  }
};

#endif