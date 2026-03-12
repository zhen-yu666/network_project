#pragma once

#ifndef CONNECTION_H
#define CONNECTION_H

#include <base/timestamp.h>
#include "base/buffer.h"
#include "base/socket.h"

#include <atomic>
#include <functional>
#include <memory>

#include <sys/syscall.h>
#include <unistd.h>

class EventLoop;
class Socket;
class Channel;

class Connection;
using SptrConnection = std::shared_ptr<Connection>;

class Connection : public std::enable_shared_from_this<Connection> {
private:
  // Connection对应的事件循环，在构造函数中传入。
  EventLoop* loop_ = nullptr;
  // 与客户端通讯的Socket。
  std::unique_ptr<Socket> client_sock_;
  // Connection对应的channel，在构造函数中创建。
  std::unique_ptr<Channel> client_channel_;
  // 用户输入缓冲区
  Buffer input_buffer_;
  // 用户输出缓冲区
  Buffer output_buffer_;
  // 客户端连接是否已断开，如果已断开，则设置为true。
  std::atomic_bool disconnect_ = false;

  // 时间戳，创建Connection对象时为当前时间
  // 每接收到一个报文，把时间戳更新为当前时间。
  Timestamp last_receive_time_;

  // 关闭fd_的回调函数
  // 将回调TcpServer::closeconnection()。
  std::function<void(SptrConnection)> closeCallback_;
  // fd_发生了错误的回调函数
  // 将回调TcpServer::errorconnection()。
  std::function<void(SptrConnection)> errorCallback_;
  // 处理报文的回调函数
  // 将回调TcpServer::onmessage()。
  std::function<void(SptrConnection, const std::string&)> onMsgCallback_;
  // 发送数据完成后的回调函数
  // 将回调TcpServer::sendcomplete()。
  std::function<void(SptrConnection)> sendCompleteCallback_;

private:
  void init();

  // 在 IO 线程中实际执行发生操作
  void sendInLoop(const std::string& message) {
    // 复用已有的发送逻辑（注意 message 可能较大，考虑移动）
    printf("Connection::send() thread is %ld.\n", syscall(SYS_gettid));
    send(message.data(), message.size());
  }

public:
  Connection(EventLoop* loop, std::unique_ptr<Socket> client_sock)
      : loop_(loop), client_sock_(std::move(client_sock)) {
    init();
  }

  ~Connection() = default;

  // 返回fd_成员。
  const int fd() const { return client_sock_->fd(); }

  // 返回ip_成员。
  const std::string& ip() const { return client_sock_->ip(); }

  // 返回port_成员。
  uint16_t port() const { return client_sock_->port(); }

  // 处理对端发送到服务端的信息。
  void onMessage();

  // 发送数据
  void send(const char* data, size_t size);

  // 工作线程直接调用此方法
  void send(const std::string& message);

  // TCP连接关闭（断开）的回调函数，供Channel回调。
  void closeCallback();

  // TCP连接错误的回调函数，供Channel回调。
  void errorCallback();

  // 处理写事件的回调函数，供Channel回调。
  void writeCallback();

  // 更新最后活动时间（每次收到数据后调用）
  void updateLastReceiveTime() { last_receive_time_ = Timestamp::now(); }

  // 判断是否空闲超时
  bool isIdle(double timeoutSeconds) const {
    return last_receive_time_.isTimeout(timeoutSeconds);
  }

  // 设置关闭fd_的回调函数。
  template<typename Callback>
  void setCloseCallback(Callback&& cb) {
    closeCallback_ = std::forward<Callback>(cb);
  }

  // 设置fd_发生了错误的回调函数。
  template<typename Callback>
  void setErrorCallback(Callback&& cb) {
    errorCallback_ = std::forward<Callback>(cb);
  }

  // 设置fd_发生了错误的回调函数。
  template<typename Callback>
  void setOnMsgCallback(Callback&& cb) {
    onMsgCallback_ = std::forward<Callback>(cb);
  }

  // 发送数据完成后的回调函数。
  template<typename Callback>
  void setSendCompleteCallback(Callback&& cb) {
    sendCompleteCallback_ = std::forward<Callback>(cb);
  }
};

#endif