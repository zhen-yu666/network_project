#include "utility/inet_address.h"

#include <arpa/inet.h>

InetAddress::InetAddress() {}

InetAddress::InetAddress(const std::string& ip, uint16_t port) {
  // IPV4网络协议的套接字类型。
  addr_.sin_family = ADDRESS_P;
  // 服务端用于监听的IP地址。
  addr_.sin_addr.s_addr = inet_addr(ip.c_str());
  // 服务端用于监听的端口。
  addr_.sin_port = htons(port);
}

InetAddress::InetAddress(const sockaddr_in& addr) : addr_(addr) {}

InetAddress::~InetAddress() {}

const char*
InetAddress::ip() const {
  return inet_ntoa(addr_.sin_addr);
}

uint16_t
InetAddress::port() const {
  return ntohs(addr_.sin_port);
}

const sockaddr*
InetAddress::addr() const {
  return reinterpret_cast<const sockaddr*>(&addr_);
}

void
InetAddress::setaddr(const sockaddr_in& client_addr) {
  addr_ = client_addr;
}