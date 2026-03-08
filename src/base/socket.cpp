#include "base/socket.h"

#include <unistd.h>

#ifdef SOCKET_DEBUG

#include <cstdio>

#define PRINTF()                                                             \
  printf("%s:%s:%d listen socket create error:%d\n", __FILE__, __FUNCTION__, \
         __LINE__, errno)

#endif  // SOCKET_DEBUG

int
createNonbloking() {
  // 创建服务端用于监听的listenfd。
  int listen_fd = socket(SOCKET_DOMAIN, SOCKET_TYPE, SOCKET_PROTOCOL);
  if(listen_fd < 0) {
#ifdef SOCKET_DEBUG
    PRINTF();
#endif  // SOCKET_DEBUG
    exit(-1);
  }
  return listen_fd;
}

Socket::Socket(int fd) : fd_(fd), ip_(""), port_(0) {}

Socket::~Socket() {
  close(fd_);
}

const int
Socket::fd() const {
  return fd_;
}

const std::string&
Socket::ip() const {
  return ip_;
}

uint16_t
Socket::port() const {
  return port_;
}

void
Socket::setIp(const std::string& ip) {
  ip_ = ip;
}

void
Socket::setPort(uint16_t port) {
  port_ = port;
}

void
Socket::setReuseaddr(bool on) {
  int opt = on ? 1 : 0;
  setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

void Socket::setReuseport(bool on) {}

void Socket::setTcpNodelay(bool on) {}

void Socket::setKeepalive(bool on) {}

void Socket::bind(const InetAddress& serv_addr) {}

void Socket::listen(int n) {}

int Socket::accept(InetAddress& client_addr) {}