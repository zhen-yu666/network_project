#include "net/acceptor.h"

#include "base/channel.h"
#include "base/socket.h"
#include "net/connection.h"


void
Acceptor::init(const std::string& ip, const uint16_t port) {
  listen_sock_ = new Socket((createNonbloking()));
  InetAddress servaddr(ip, port);
  listen_sock_->setReuseaddr(true);
  listen_sock_->setTcpNodelay(true);
  listen_sock_->setReuseport(true);
  listen_sock_->setKeepalive(true);
  listen_sock_->bind(servaddr);
  listen_sock_->listen();

  listen_channel_ = new Channel(loop_, listen_sock_->fd());
  // 新连接处理
  listen_channel_->setReadCallback(std::bind(&Acceptor::newConnection, this));
  // 将监听的socket挂在树上
  listen_channel_->enableReading();
}

Acceptor::~Acceptor() {
  delete listen_sock_;
  listen_sock_ = nullptr;
  delete listen_channel_;
  listen_channel_ = nullptr;
}

void
Acceptor::newConnection() {
  // 客户端的地址和协议。
  InetAddress clientaddr;
  // 注意，clientsock只能new出来，不能在栈上，否则析构函数会关闭fd。
  // 还有，这里new出来的对象没有释放，这个问题以后再解决。
  Socket* clientsock =
    new Socket(listen_sock_->accept4(clientaddr, SOCK_NONBLOCK));

  // 将新连接的客户端挂在树上。
  // 回调TcpServer::newconnection()
  new_conn_callback_(clientsock);
}