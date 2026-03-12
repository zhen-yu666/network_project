#include "net/tcp_server.h"

#include "base/socket.h"
#include "net/connection.h"

#include <unistd.h>
#include <cstring>

void
TcpServer::init(const std::string& ip, const uint16_t port) {
  // 创建主事件循环。
  main_loop_ = new EventLoop;
  // 设置timeout超时的回调函数。
  main_loop_->setEpollTimeoutCallback(
    std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));

  acceptor_ = new Acceptor(main_loop_, ip, port);

  acceptor_->setNewConnCallback(
    std::bind(&TcpServer::newConnection, this, std::placeholders::_1));

  // 创建线程池
  thread_pool_ = new ThreadPool(thread_num_);

  // 创建从事件循环。
  for(int i = 0; i < thread_num_; ++i) {
    // 创建从事件循环，存入subloops_容器中。
    sub_loops_.emplace_back(new EventLoop);
    // 设置timeout超时的回调函数。
    sub_loops_[i]->setEpollTimeoutCallback(
      std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));
    // 在线程池中运行从事件循环。
    thread_pool_->addTask(std::bind(&EventLoop::run, sub_loops_[i].get()));
  }
}

TcpServer::~TcpServer() {
  delete main_loop_;
  main_loop_ = nullptr;

  delete thread_pool_;
  thread_pool_ = nullptr;

  delete acceptor_;
  acceptor_ = nullptr;
}

void
TcpServer::start() {
  main_loop_->run();
}

/* 
void
TcpServer::newConnection(std::unique_ptr<Socket> client_sock) {
  // 如果构造函数内部调用了 shared_from_this()，可能在此处抛出异常
  // 这里是先 new 对象，然后再关联。而使用 make_shared 是直接一步到位
  // SptrConnection conn(new Connection(
  //   sub_loops_[client_sock->fd() % thread_num_], std::move(client_sock)));
  // 后续可以采用负载均衡的方式，让从事件循环分配均匀
  SptrConnection conn = std::make_shared<Connection>(
    sub_loops_[client_sock->fd() % thread_num_].get(), std::move(client_sock));
  conn->setCloseCallback(
    std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
  conn->setErrorCallback(
    std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));
  conn->setOnMsgCallback(std::bind(
    &TcpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
  conn->setSendCompleteCallback(
    std::bind(&TcpServer::sendComplete, this, std::placeholders::_1));

  conns_.insert({conn->fd(), conn});

  if(new_conn_callback_)
    new_conn_callback_(conn);
} */

void
TcpServer::newConnection(std::unique_ptr<Socket> client_sock) {
  // 选择子 EventLoop（负载均衡）
  int idx = client_sock->fd() % thread_num_;
  EventLoop* sub_loop = sub_loops_[idx].get();

  // 从 client_sock 中提取原始指针，并释放其所有权
  // release() 返回原始指针，原 unique_ptr 变为空，不会在函数结束时 delete 该对象
  Socket* raw_sock = client_sock.release();

  // 将原始指针捕获到 lambda 中（原始指针可拷贝，lambda 变为可拷贝类型）
  // 同时捕获其他所需变量（this, sub_loop, idx）
  sub_loop->queueInLoop([this, sub_loop, raw_sock, idx]() {
    // 现在这段代码在子线程中执行（因为是通过 queueInLoop 投递的）

    // 在子线程中，用原始指针重新构造 unique_ptr，重新获得所有权
    // 这样 raw_sock 指向的 Socket 对象将由这个新 unique_ptr 管理
    std::unique_ptr<Socket> sock(raw_sock);

    // 创建 Connection 对象，将 sock 的所有权转移给 Connection
    SptrConnection conn =
      std::make_shared<Connection>(sub_loop, std::move(sock));

    // 设置各种回调（这些回调将在子线程中触发）
    conn->setCloseCallback(
      std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
    conn->setErrorCallback(
      std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));
    conn->setOnMsgCallback(std::bind(&TcpServer::onMessage, this,
                                     std::placeholders::_1,
                                     std::placeholders::_2));
    conn->setSendCompleteCallback(
      std::bind(&TcpServer::sendComplete, this, std::placeholders::_1));

    // 将新连接加入主线程的映射表（conns_ 由主线程独占访问）
    // 因此需要再通过主线程的 queueInLoop 抛回主线程执行
    main_loop_->queueInLoop([this, conn]() {
      // 由主线程安全地插入
      conns_[conn->fd()] = conn;
      // 回调 EchoServer 的新连接处理
      if(new_conn_callback_)
        new_conn_callback_(conn);
    });

    // 此时 conn 的构造函数已经在子线程执行完毕，其中的 enableReading() 也在子线程中调用，
    // 因此 epoll_ctl 操作是在正确的线程中完成的，不会再导致跨线程竞争。
    // 而将连接插入 conns_ 的操作通过主线程的队列完成，也保证了线程安全。
  });
  // 原 client_sock 已 release，此处析构时不会删除 Socket 对象
}

void
TcpServer::closeConnection(SptrConnection conn) {
  // 这个函数可能被子线程调用，不能直接修改 conns_，需要抛给主线程
  main_loop_->queueInLoop([this, conn] {
    // 在主线程中安全删除
    conns_.erase(conn->fd());
    if(close_conn_callback_) {
      // 回调 EchoServer
      close_conn_callback_(conn);
    }
  });
}

void
TcpServer::errorConnection(SptrConnection conn) {
  main_loop_->queueInLoop([this, conn] {
    // 在主线程中安全删除
    conns_.erase(conn->fd());
    if(error_conn_callback_) {
      error_conn_callback_(conn);
    }
  });
}

void
TcpServer::onMessage(SptrConnection conn, const std::string& msg) {
  if(on_msg_callback_)
    on_msg_callback_(conn, msg);
}

void
TcpServer::sendComplete(SptrConnection conn) {
  if(send_complete_callback_)
    send_complete_callback_(conn);
}

void
TcpServer::epollTimeout(EventLoop* loop) {
  if(timeout_callback_)
    timeout_callback_(loop);
}