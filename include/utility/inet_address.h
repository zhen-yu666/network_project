/**
 * @file inet_address.h
 * @brief socket的地址协议类。
 * @author  ()
 * @date 2026-03-07
 * 
 * @copyright Copyright (c) 2026
 * 
*/

#pragma once

#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

#include <netinet/in.h>
#include <string>

#define ADDRESS_P AF_INET

class InetAddress {
private:
  // 表示地址协议结构体
  sockaddr_in addr_;

public:
  InetAddress();

  // 如果是监听的fd，用这个构造函数
  InetAddress(const std::string& ip, uint16_t port);

  // 如果是客户端连上来的fd，用这个构造函数
  InetAddress(const sockaddr_in& addr);

  ~InetAddress();

  // 返回字符串表示的地址，例如：192.168.150.128
  const char* ip() const;

  // 返回整数表示的端口，例如：80、8080
  uint16_t port() const;

  // 返回addr_成员的地址，转换成了sockaddr。
  const sockaddr* addr() const;

  // 设置addr_成员的值。
  void setaddr(const sockaddr_in& client_addr);
};

#endif  // INET_ADDRESS_H