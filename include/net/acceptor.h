#pragma once

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "base/channel.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

class Socket;
class Channel;
class EventLoop;

class Acceptor {
private:
  // Acceptor对应的事件循环，在构造函数中传入。
  EventLoop* loop_ = nullptr;
  // 服务端用于监听的socket，在构造函数中创建。
  std::unique_ptr<Socket> listen_sock_;
  // Acceptor对应的channel，在构造函数中创建。
  std::unique_ptr<Channel> listen_channel_;
  // 处理新客户端连接请求的回调函数。
  std::function<void(std::unique_ptr<Socket>)> new_conn_callback_;

private:
  void init(const std::string& ip, const uint16_t port);

public:
  Acceptor(EventLoop* loop, const std::string& ip, const uint16_t port)
      : loop_(loop), listen_sock_(nullptr), listen_channel_(nullptr) {
    init(std::move(ip), port);
  }

  ~Acceptor() = default;

  // 处理新客户端连接请求。
  void newConnection();

  // 设置处理新客户端连接请求的回调函数。
  template<typename Callback>
  void setNewConnCallback(Callback&& cb) {
    new_conn_callback_ = std::forward<Callback>(cb);
  }
};

#endif