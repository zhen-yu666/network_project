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
  Channel* listen_channel = new Channel(listen_sock.fd(), &ep);
  // 让epoll_wait()监视servchannel的读事件。
  listen_channel->enableReading();

  // 事件循环。
  while(true) {
    std::vector<Channel*> channels = std::move(ep.loop());

    // 如果infds>0，表示有事件发生的fd的数量。
    // 遍历epoll返回的数组evs。
    for(auto ch : channels) {
/* 
      if(ch->getRevents() & EPOLLRDHUP) {
        // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
        printf("client(eventfd=%d) disconnected.\n", ch->getFd());
        close(ch->getFd());  // 关闭客户端的fd。
      } else if(ch->getRevents() & (EPOLLIN | EPOLLPRI)) {
        // 接收缓冲区中有数据可以读。
        if(ch == listen_channel) {
          // 如果是listenfd有事件，表示有新的客户端连上来。
          ////////////////////////////////////////////////////////////////////////
          // 客户端的地址和协议。
          InetAddress clientaddr;
          // 注意，clientsock只能new出来，不能在栈上，否则析构函数会关闭fd。
          // 还有，这里new出来的对象没有释放，这个问题以后再解决。
          Socket* clientsock =
            new Socket(listen_sock.accept4(clientaddr, SOCK_NONBLOCK));

          printf("accept client(fd=%d,ip=%s,port=%d) ok.\n", clientsock->fd(),
                 clientaddr.ip(), clientaddr.port());

          // 这里new出来的对象没有释放，这个问题以后再解决。
          Channel* client_channel = new Channel(clientsock->fd(), &ep);
          // 客户端连上来的fd采用边缘触发。
          client_channel->useET();
          // 让epoll监视clientchannel的读事件。
          client_channel->enableReading();
          ////////////////////////////////////////////////////////////////////////
        } else {
          // 如果是客户端已连接的fd有事件。
          ////////////////////////////////////////////////////////////////////////
          char buffer[1024];
          // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
          while(true) {
            memset(buffer, 0, sizeof(buffer));
            ssize_t nread = read(ch->getFd(), buffer, sizeof(buffer));
            if(nread > 0) {
              // 成功的读取到了数据。
              // 把接收到的报文内容原封不动的发回去。
              printf("recv(eventfd=%d):%s\n", ch->getFd(), buffer);
              send(ch->getFd(), buffer, strlen(buffer), 0);
            } else if(nread == -1 && errno == EINTR) {
              // 读取数据的时候被信号中断，继续读取。
              continue;
            } else if(nread == -1
                      && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
              // 全部的数据已读取完毕。
              break;
            } else if(nread == 0) {
              // 客户端连接已断开。
              printf("client(eventfd=%d) disconnected.\n", ch->getFd());
              close(ch->getFd());  // 关闭客户端的fd。
              break;
            }
          }
          ////////////////////////////////////////////////////////////////////////
        }
      } else if(ch->getRevents() & EPOLLOUT) {
        // 有数据需要写，暂时没有代码，以后再说。
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
      } else {
        // 其它事件，都视为错误。
        ////////////////////////////////////////////////////////////////////////
        printf("client(eventfd=%d) error.\n", ch->getFd());
        close(ch->getFd());  // 关闭客户端的fd。
        ////////////////////////////////////////////////////////////////////////
      } */

      ch->handLevent(&listen_sock);
    }
  }

  return 0;
}