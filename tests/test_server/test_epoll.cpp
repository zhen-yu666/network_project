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

  Epoll listen_ep;
  // 让epoll监视listenfd的读事件，采用水平触发。
  listen_ep.addFd(listen_sock.fd(), EPOLLIN);
  // 存放epoll_wait()返回事件。
  std::vector<epoll_event> evs;

  // 事件循环。
  while(true) {
    evs = std::move(listen_ep.loop());

    // 如果infds>0，表示有事件发生的fd的数量。
    // 遍历epoll返回的数组evs。
    for(auto& ev : evs) {
      if(ev.events & EPOLLRDHUP) {
        // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
        printf("client(eventfd=%d) disconnected.\n", ev.data.fd);
        close(ev.data.fd);  // 关闭客户端的fd。
      } else if(ev.events & (EPOLLIN | EPOLLPRI)) {
        // 接收缓冲区中有数据可以读。
        if(ev.data.fd == listen_sock.fd()) {
          // 如果是listenfd有事件，表示有新的客户端连上来。
          ////////////////////////////////////////////////////////////////////////
          InetAddress clientaddr;  // 客户端的地址和协议。
          // 注意，clientsock只能new出来，不能在栈上，否则析构函数会关闭fd。
          // 还有，这里new出来的对象没有释放，这个问题以后再解决。
          Socket* clientsock =
            new Socket(listen_sock.accept4(clientaddr, SOCK_NONBLOCK));

          printf("accept client(fd=%d,ip=%s,port=%d) ok.\n", clientsock->fd(),
                 clientaddr.ip(), clientaddr.port());

          // 客户端连上来的fd采用边缘触发。
          listen_ep.addFd(clientsock->fd(), EPOLLIN | EPOLLET);
          ////////////////////////////////////////////////////////////////////////
        } else {
          // 如果是客户端已连接的fd有事件。
          ////////////////////////////////////////////////////////////////////////
          char buffer[1024];
          // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
          while(true) {
            bzero(&buffer, sizeof(buffer));
            ssize_t nread = read(ev.data.fd, buffer, sizeof(buffer));
            if(nread > 0) {
              // 成功的读取到了数据。
              // 把接收到的报文内容原封不动的发回去。
              printf("recv(eventfd=%d):%s\n", ev.data.fd, buffer);
              send(ev.data.fd, buffer, strlen(buffer), 0);
            } else if(nread == -1 && errno == EINTR) {
              // 读取数据的时候被信号中断，继续读取。
              continue;
            } else if(nread == -1
                      && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
              // 全部的数据已读取完毕。
              break;
            } else if(nread == 0) {
              // 客户端连接已断开。
              printf("client(eventfd=%d) disconnected.\n", ev.data.fd);
              close(ev.data.fd);  // 关闭客户端的fd。
              break;
            }
          }
        }
      } else if(ev.events & EPOLLOUT) {
        // 有数据需要写，暂时没有代码，以后再说。
      } else {
        // 其它事件，都视为错误。
        printf("client(eventfd=%d) error.\n", ev.data.fd);
        close(ev.data.fd);  // 关闭客户端的fd。
      }
    }
  }

  return 0;
}