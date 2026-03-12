#include "server/echo_server.h"

#include <sys/syscall.h>
#include <unistd.h>
#include <iostream>

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
EchoServer::Start() {
  tcpserver_.start();
}

void
EchoServer::handleNewConnection(SptrConnection conn) {
  std::cout << "New Connection Come in." << std::endl;

  printf("EchoServer::HandleNewConnection() thread is %ld.\n",
         syscall(SYS_gettid));

  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::handleClose(SptrConnection conn) {
  std::cout << "EchoServer conn closed." << std::endl;

  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::handleError(SptrConnection conn) {
  std::cout << "EchoServer conn error." << std::endl;

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
  std::cout << "Message send complete." << std::endl;

  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::handleTimeout(EventLoop* loop) {
  std::cout << "EchoServer timeout." << std::endl;

  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::onMessage(SptrConnection conn, const std::string& message) {
  printf("EchoServer::onMessage() thread is %ld.\n", syscall(SYS_gettid));
  // 构造响应数据
  std::string reply = "reply:" + message;
  // 把临时缓冲区中的数据发送出去
  // 自动跨线程投递
  conn->send(std::move(reply));
}