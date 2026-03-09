#pragma once

#ifndef CHANNEL_H
#define CHANNEL_H

#include <sys/epoll.h>
#include <cstdint>

class Epoll;
class Socket;

enum class ChannelType : uint8_t {
  // 监听套接字
  ListenFd,
  // 已连接套接字
  ConnFd
};

class Channel {
private:
  // Channel拥有的fd，Channel和fd是一对一的关系。
  int fd_ = -1;
  // fd_需要监视的事件。listenfd和clientfd需要监视EPOLLIN，clientfd还可能需要监视EPOLLOUT。
  uint32_t events_ = 0;
  // fd_已发生的事件。
  uint32_t revents_ = 0;
  // 当前套接字是否在epoll红黑树上。
  bool inEpoll_ = false;
  // 套接字属于什么类型。
  ChannelType type_ = ChannelType::ListenFd;
  // Channel对应的红黑树，Channel与Epoll是多对一的关系，一个Channel只对应一个Epoll。
  Epoll* ep_ = nullptr;

public:
  Channel(int fd, ChannelType type, Epoll* ep)
      : fd_(fd), type_(type), ep_(ep) {}

  ~Channel() = default;

  // 采用边缘触发。
  void useET() { events_ |= EPOLLET; }

  // 让epoll_wait()监视fd_的读事件。
  void enableReading();

  // 返回fd_成员。
  int getFd() { return fd_; }

  // 返回inepoll_成员。
  bool getInEpoll() { return inEpoll_; }

  // 把inepoll_成员的值设置为true。
  void setInEpoll() { inEpoll_ = true; }

  // 返回revents_成员。
  uint32_t getRevents() { return revents_; }

  // 设置revents_成员的值为参数ev。
  void setRevents(uint32_t ev) { revents_ = ev; }

  // 返回events_成员。
  uint32_t getEvents() { return events_; }

  // 事件处理函数。
  void handLevent(Socket* serv_sock);
};

#endif