#include "base/channel.h"
#include "base/epoll.h"
#include "base/socket.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#define PRINTF(fmt, ...) \
  printf("%s:%s:%d" fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

int
main(int argc, char* argv[]) {
  if(argc != 3) {
    printf("usage: ./tcpepoll ip port\n");
    return -1;
  }

  Socket listen_sock(createNonbloking());
  InetAddress servaddr(argv[1], atoi(argv[2]));  // 服务端的地址和协议。
  listen_sock.setReuseaddr(true);
  listen_sock.setTcpNodelay(true);
  listen_sock.setReuseport(true);
  listen_sock.setKeepalive(true);
  listen_sock.bind(servaddr);
  listen_sock.listen();

  Epoll ep;
  // 这里new出来的对象没有释放，这个问题以后再解决。
  Channel* listen_channel =
    new Channel(listen_sock.fd(), ChannelType::ListenFd, &ep);
  // 让epoll_wait()监视servchannel的读事件。
  listen_channel->enableReading();

  // 事件循环。
  while(true) {
    std::vector<Channel*> channels = std::move(ep.loop());

    // 如果infds>0，表示有事件发生的fd的数量。
    // 遍历epoll返回的数组evs。
    for(auto ch : channels) {
      ch->handLevent(&listen_sock);
    }
  }

  return 0;
}