#include "server/echo_server.h"

#include <sys/syscall.h>
#include <unistd.h>

void
EchoServer::init() {
  tcpserver_.setNewConnCallback(
    [this](SptrConnection conn) { handleNewConnection(conn); });
  tcpserver_.setCloseConnCallback(
    [this](SptrConnection conn) { handleClose(conn); });
  tcpserver_.setErrorConnCallback(
    [this](SptrConnection conn) { handleError(conn); });
  tcpserver_.setOnMsgCallback(
    [this](SptrConnection conn, const std::string& message) {
      handleMessage(conn, std::move(message));
    });
  tcpserver_.setSendCompleteCallback(
    [this](SptrConnection conn) { handleSendComplete(conn); });
  // tcpserver_.setTimeoutCallback(
  //   [this](EventLoop* loop) { handleTimeout(loop); });
}

void
EchoServer::start() {
  tcpserver_.start();
}

void
EchoServer::stop() {
  // 停止工作线程。
  threads_->stop();
  printf("工作线程已停止。\n");

  // 停止IO线程（事件循环）。
  tcpserver_.stop();
}

void
EchoServer::handleNewConnection(SptrConnection conn) {
  printf("new connection(fd=%d,ip=%s,port=%d) ok.\n", conn->fd(),
         conn->ip().c_str(), conn->port());
  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::handleClose(SptrConnection conn) {
  printf("connection closed(fd=%d,ip=%s,port=%d).\n", conn->fd(),
         conn->ip().c_str(), conn->port());
  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::handleError(SptrConnection conn) {
  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::handleMessage(SptrConnection conn, const std::string& message) {
  if(threads_->size() == 0) {
    onMessage(conn, std::move(message));
  } else {
    // 把业务添加到线程池的任务队列中。
    threads_->addTask([this, conn, msg = std::move(message)]() {
      onMessage(conn, std::move(msg));
    });
  }
}

void
EchoServer::handleSendComplete(SptrConnection conn) {
  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::handleTimeout(EventLoop* loop) {
  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::onMessage(SptrConnection conn, const std::string& message) {
  // 构造响应数据
  std::string reply = "reply:" + message;
  // 把临时缓冲区中的数据发送出去
  // 自动跨线程投递
  conn->send(std::move(reply));
}