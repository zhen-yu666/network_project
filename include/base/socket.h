/**
 * @file socket.h
 * @brief 封装了初始化socket的过程
 * @author  ()
 * @date 2026-03-07
 * 
 * @copyright Copyright (c) 2026
 * 
*/

#pragma once

#ifndef SOCKET_H
#define SOCKET_H

#include "base/inet_address.h"

#include <cstdint>
#include <string>

#define SOCKET_DOMAIN AF_INET
#define SOCKET_TYPE (SOCK_STREAM|SOCK_NONBLOCK)
#define SOCKET_PROTOCOL IPPROTO_TCP

// 创建一个非阻塞的socket。
int createNonbloking();

class Socket {
private:
  // Socket持有的fd
  const int fd_;
  // 如果是listenfd，存放服务端监听的ip，如果是客户端连接的fd，存放对端的ip。
  std::string ip_;
  // 如果是listenfd，存放服务端监听的port，如果是客户端连接的fd，存放外部端口。
  uint16_t port_;

public:
  // 构造函数，传入一个已准备好的fd。
  Socket(int fd);

  // 在析构函数中，将关闭fd_。
  ~Socket();

  // 返回fd_成员。
  const int fd() const;

  // 返回ip_成员。
  const std::string& ip() const;

  // 返回port_成员。
  uint16_t port() const;

  // 设置ip_成员。
  void setIp(const std::string& ip);

  // 设置port_成员。
  void setPort(uint16_t port);

  // 设置SO_REUSEADDR选项，true-打开，false-关闭。
  void setReuseaddr(bool on);

  // 设置SO_REUSEPORT选项。
  void setReuseport(bool on);

  // 设置TCP_NODELAY选项。
  void setTcpNodelay(bool on);

  // 设置SO_KEEPALIVE选项。
  void setKeepalive(bool on);

  // 服务端的socket将调用此函数。
  void bind(const InetAddress& serv_addr);

  // 服务端的socket将调用此函数。
  void listen(int n = 128);

  // 服务端的socket将调用此函数。
  int accept(InetAddress& client_addr);
};

#endif  // SOCKET_H