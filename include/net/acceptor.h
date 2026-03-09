#pragma once

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <cstdint>
#include <functional>
#include <string>

class Socket;
class Channel;
class EventLoop;

class Acceptor {
private:
  // Acceptor对应的事件循环，在构造函数中传入。
  EventLoop* loop_ = nullptr;
  // 服务端用于监听的socket，在构造函数中创建。
  Socket* listen_sock_ = nullptr;
  // Acceptor对应的channel，在构造函数中创建。
  Channel* listen_channel_ = nullptr;
  // 处理新客户端连接请求的回调函数。
  std::function<void(Socket*)> new_conn_callback_;

private:
  void init(const std::string& ip, const uint16_t port);

public:
  template<typename Ip, typename Port>
  Acceptor(EventLoop* loop, Ip&& ip, Port& port) : loop_(loop) {
    init(std::forward<Ip>(ip), std::forward<Port>(port));
  }

  ~Acceptor();

  // 处理新客户端连接请求。
  void newConnection();

  // 设置处理新客户端连接请求的回调函数。
  template<typename Callback>
  void setNewConnCallback(Callback&& cb) {
    new_conn_callback_ = std::forward<Callback>(cb);
  }
};

#endif