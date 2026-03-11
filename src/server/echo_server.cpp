#include "server/echo_server.h"

#include <iostream>

void
EchoServer::init() {
  tcpserver_.setNewConnCallback(
    std::bind(&EchoServer::handleNewConnection, this, std::placeholders::_1));
  tcpserver_.setCloseConnCallback(
    std::bind(&EchoServer::handleClose, this, std::placeholders::_1));
  tcpserver_.setErrorConnCallback(
    std::bind(&EchoServer::handleError, this, std::placeholders::_1));
  tcpserver_.setOnMsgCallback(std::bind(&EchoServer::handleMessage, this,
                                        std::placeholders::_1,
                                        std::placeholders::_2));
  tcpserver_.setSendCompleteCallback(
    std::bind(&EchoServer::handleSendComplete, this, std::placeholders::_1));
  tcpserver_.setTimeoutCallback(
    std::bind(&EchoServer::handleTimeout, this, std::placeholders::_1));
}

void
EchoServer::Start() {
  tcpserver_.start();
}

void
EchoServer::handleNewConnection(Connection* conn) {
  std::cout << "New Connection Come in." << std::endl;

  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::handleClose(Connection* conn) {
  std::cout << "EchoServer conn closed." << std::endl;

  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::handleError(Connection* conn) {
  std::cout << "EchoServer conn error." << std::endl;

  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::handleMessage(Connection* conn, const std::string& message) {
  // 构造响应数据
  std::string reply = "reply:" + message;
  // 计算回应报文的大小
  int len = reply.size();
  // 把报文头部填充到回应报文中
  std::string tmp_buf((char*)&len, 4);
  // 把报文内容填充到回应报文中
  tmp_buf.append(reply);

  // 把临时缓冲区中的数据发送出去
  conn->send(tmp_buf.data(), tmp_buf.size());
}

void
EchoServer::handleSendComplete(Connection* conn) {
  std::cout << "Message send complete." << std::endl;

  // 根据业务的需求，在这里可以增加其它的代码。
}

void
EchoServer::handleTimeout(EventLoop* loop) {
  std::cout << "EchoServer timeout." << std::endl;

  // 根据业务的需求，在这里可以增加其它的代码。
}