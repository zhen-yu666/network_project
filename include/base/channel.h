#pragma once

#ifndef CHANNEL_H
#define CHANNEL_H

#include <sys/epoll.h>
#include <cstdint>

class Epoll;

class Channel {
private:
  // Channel拥有的fd，Channel和fd是一对一的关系。
  int fd_ = -1;
  // fd_需要监视的事件。listenfd和clientfd需要监视EPOLLIN，clientfd还可能需要监视EPOLLOUT。
  uint32_t events_ = 0;
  // fd_已发生的事件。
  uint32_t revents_ = 0;
  // Channel是否已添加到epoll树上，如果未添加，调用epoll_ctl()的时候用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD。
  bool inEpoll_ = false;
  // Channel对应的红黑树，Channel与Epoll是多对一的关系，一个Channel只对应一个Epoll。
  Epoll* ep_ = nullptr;

public:
  Channel(int fd, Epoll* ep) : fd_(fd), ep_(ep) {}

  ~Channel() = default;

  // 返回fd_成员。
  int getFd() { return fd_; }

  // 采用边缘触发。
  void useET() { events_ |= EPOLLET; }

  // 让epoll_wait()监视fd_的读事件。
  void enableReading();

  // 把inepoll_成员的值设置为true。
  void setInEpoll() { inEpoll_ = true; }

  // 返回inepoll_成员。
  bool getInEpoll() { return inEpoll_; }

  // 设置revents_成员的值为参数ev。
  void setRevents(uint32_t ev) { revents_ = ev; }

  // 返回revents_成员。
  uint32_t getRevents() { return revents_; }

  // 返回events_成员。
  uint32_t getEvents() { return events_; }
};

#endif