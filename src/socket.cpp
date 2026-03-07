#include "socket.h"

#include <unistd.h>

int
createNonbloking() {}

Socket::Socket(int fd) : fd_(fd), ip_(""), port_(0) {}

Socket::~Socket() {
  close(fd_);
}

// 返回fd_成员。
int Socket::fd() const;

// 返回ip_成员。
const std::string& Socket::ip() const;

// 返回port_成员。
uint16_t Socket::port() const;

// 设置ip_成员。
void Socket::setIp(const std::string& ip);

// 设置port_成员。
void Socket::setPort(uint16_t port);

// 设置SO_REUSEADDR选项，true-打开，false-关闭。
void Socket::setReuseaddr(bool on);

// 设置SO_REUSEPORT选项。
void Socket::setReuseport(bool on);

// 设置TCP_NODELAY选项。
void Socket::setTcpNodelay(bool on);

// 设置SO_KEEPALIVE选项。
void Socket::setKeepalive(bool on);

// 服务端的socket将调用此函数。
void Socket::bind(const InetAddress& serv_addr);

// 服务端的socket将调用此函数。
void Socket::listen(int n = 128);

// 服务端的socket将调用此函数。
int Socket::accept(InetAddress& client_addr);