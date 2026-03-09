#pragma once

#ifndef CONNECTION_H
#define CONNECTION_H

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

private:
  void init();

public:
  Connection(EventLoop* loop, Socket* client_sock_)
      : loop_(loop), client_sock_(client_sock_) {
    init();
  }

  ~Connection();
};

#endif