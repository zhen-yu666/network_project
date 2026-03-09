#include "base/channel.h"
#include "base/epoll.h"
#include "base/inet_address.h"
#include "base/socket.h"

#include <unistd.h>
#include <cstring>

// #define CHANNEL_DEBUG 1
#ifdef CHANNEL_DEBUG

#include <cstdio>

#define PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__);

#endif

void
Channel::enableReading() {
  events_ |= EPOLLIN;
  ep_->updateChannel(this);
}

void
Channel::handLevent(Socket* serv_sock) {
  if(revents_ & EPOLLRDHUP) {
    // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。

#ifdef CHANNEL_DEBUG
    PRINTF("client(eventfd=%d) disconnected.\n", fd_);
#endif

    close(fd_);  // 关闭客户端的fd。
  } else if(revents_ & (EPOLLIN | EPOLLPRI)) {
    // 接收缓冲区中有数据可以读。
    if(type_ == ChannelType::ListenFd) {
      // 监听的套接字有事件。
      ////////////////////////////////////////////////////////////////////////
      // 客户端的地址和协议。
      InetAddress clientaddr;
      // 注意，clientsock只能new出来，不能在栈上，否则析构函数会关闭fd。
      // 还有，这里new出来的对象没有释放，这个问题以后再解决。
      Socket* clientsock =
        new Socket(serv_sock->accept4(clientaddr, SOCK_NONBLOCK));

#ifdef CHANNEL_DEBUG
      PRINTF("accept client(fd=%d,ip=%s,port=%d) ok.\n", clientsock->fd(),
             clientaddr.ip(), clientaddr.port());
#endif

      // 这里new出来的对象没有释放，这个问题以后再解决。
      // 创建一个连接的fd
      Channel* client_channel =
        new Channel(clientsock->fd(), ChannelType::ConnFd, ep_);
      // 客户端连上来的fd采用边缘触发。
      client_channel->useET();
      // 让epoll监视clientchannel的读事件。
      client_channel->enableReading();
      ////////////////////////////////////////////////////////////////////////
    } else {
      // 已连接的套接字有事件。
      ////////////////////////////////////////////////////////////////////////
      char buffer[1024];
      // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
      while(true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t nread = read(fd_, buffer, sizeof(buffer));
        if(nread > 0) {
          // 成功的读取到了数据。

#ifdef CHANNEL_DEBUG
          // 把接收到的报文内容原封不动的发回去。
          PRINTF("recv(eventfd=%d):%s\n", fd_, buffer);
#endif

          send(fd_, buffer, strlen(buffer), 0);
        } else if(nread == -1 && errno == EINTR) {
          // 读取数据的时候被信号中断，继续读取。
          continue;
        } else if(nread == -1
                  && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
          // 全部的数据已读取完毕。
          break;
        } else if(nread == 0) {
          // 客户端连接已断开。

#ifdef CHANNEL_DEBUG
          PRINTF("client(eventfd=%d) disconnected.\n", fd_);
#endif

          close(fd_);  // 关闭客户端的fd。
          break;
        }
      }
      ////////////////////////////////////////////////////////////////////////
    }
  } else if(revents_ & EPOLLOUT) {
    // 有数据需要写，暂时没有代码，以后再说。
    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
  } else {
    // 其它事件，都视为错误。
    ////////////////////////////////////////////////////////////////////////

#ifdef CHANNEL_DEBUG
    PRINTF("client(eventfd=%d) error.\n", fd_);
#endif

    close(fd_);  // 关闭客户端的fd。
    ////////////////////////////////////////////////////////////////////////
  }
}