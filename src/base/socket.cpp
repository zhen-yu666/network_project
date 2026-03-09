#include "base/socket.h"

#include <netinet/tcp.h>
#include <unistd.h>

#ifdef SOCKET_DEBUG

#include <cstdio>

#define PRINTF(fmt, ...) \
  printf("%s:%s:%d" fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define PERROR(fmt) perror(fmt)

#endif  // SOCKET_DEBUG

int
createNonbloking() {
  // 创建服务端用于监听的listenfd。
  int listen_fd = socket(SOCKET_DOMAIN, SOCKET_TYPE, SOCKET_PROTOCOL);
  if(listen_fd < 0) {

#ifdef SOCKET_DEBUG
    PRINTF("msg: %d", errno);
#endif  // SOCKET_DEBUG

    exit(-1);
  }
  return listen_fd;
}

Socket::~Socket() {
  close(fd_);
}

void
Socket::setReuseaddr(bool on) {
  int opt = on ? 1 : 0;
  setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

void
Socket::setReuseport(bool on) {
  int opt = on ? 1 : 0;
  setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
}

void
Socket::setTcpNodelay(bool on) {
  int opt = on ? 1 : 0;
  setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
}

void
Socket::setKeepalive(bool on) {
  int opt = on ? 1 : 0;
  setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
}

void
Socket::bind(const InetAddress& serv_addr) {
  if(::bind(fd_, serv_addr.addr(), sizeof(sockaddr)) < 0) {

#ifdef SOCKET_DEBUG
    PERROR("bind() failed");
#endif  // SOCKET_DEBUG

    close(fd_);
    exit(-1);
  }
  ip_ = serv_addr.ip();
  port_ = serv_addr.port();
}

void
Socket::listen(int n) {
  if(::listen(fd_, n) != 0) {

#ifdef SOCKET_DEBUG
    PERROR("listen() failed");
#endif  // SOCKET_DEBUG

    close(fd_);
    exit(-1);
  }
}

int
Socket::accept(InetAddress& client_addr) {
  sockaddr_in peer_addr;
  socklen_t len = sizeof(peer_addr);
  int client_fd = ::accept(fd_, reinterpret_cast<sockaddr*>(&peer_addr), &len);

  // 客户端的地址和协议。
  client_addr.setaddr(peer_addr);

  ip_ = client_addr.ip();
  port_ = client_addr.port();

  return client_fd;
}

int
Socket::accept4(InetAddress& client_addr, int flags) {
  sockaddr_in peer_addr;
  socklen_t len = sizeof(peer_addr);
  int client_fd =
    ::accept4(fd_, reinterpret_cast<sockaddr*>(&peer_addr), &len, flags);

  // 客户端的地址和协议。
  client_addr.setaddr(peer_addr);

  ip_ = client_addr.ip();
  port_ = client_addr.port();

  return client_fd;
}