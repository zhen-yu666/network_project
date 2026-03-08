#include "base/inet_address.h"

InetAddress::InetAddress(const std::string& ip, uint16_t port) {
  // IPV4网络协议的套接字类型。
  addr_.sin_family = ADDRESS_P;
  // 服务端用于监听的IP地址。
  addr_.sin_addr.s_addr = inet_addr(ip.c_str());
  // 服务端用于监听的端口。
  addr_.sin_port = htons(port);
}