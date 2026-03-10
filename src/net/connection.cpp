#include "net/connection.h"

#include "base/channel.h"

#include <unistd.h>
#include <cstring>

void
Connection::init() {
  // 将新连接挂在树上中。
  client_channel_ = new Channel(loop_, client_sock_->fd());
  client_channel_->setReadCallback(std::bind(&Connection::onMessage, this));
  client_channel_->setCloseCallback(
    std::bind(&Connection::closeCallback, this));
  client_channel_->setErrorCallback(
    std::bind(&Connection::errorCallback, this));
  // 客户端连上来的fd采用边缘触发。
  client_channel_->useET();
  // 让epoll监视clientchannel的读事件。
  client_channel_->enableReading();
}

Connection::~Connection() {
  delete client_sock_;
  client_sock_ = nullptr;
  delete client_channel_;
  client_channel_ = nullptr;
}

void
Connection::onMessage() {
  char buffer[1024];
  // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
  while(true) {
    memset(buffer, 0, sizeof(buffer));
    ssize_t nread = read(fd(), buffer, sizeof(buffer));
    if(nread > 0) {
      // 成功的读取到了数据。
      input_buffer_.append(buffer, nread);
    } else if(nread == -1 && errno == EINTR) {
      // 读取数据的时候被信号中断，继续读取。
      continue;
    } else if(nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
      // 全部的数据已读取完毕。
      printf("read(eventfd=%d): %s\n", fd(), input_buffer_.data());
      // 在这里，将经过若干步骤的运算。
      // 运算后的结果已存放在outputbuffer_中。
      output_buffer_ = std::move(input_buffer_);
      // 把发送缓冲区中的数据直接send()出去。
      send(fd(), output_buffer_.data(), output_buffer_.size(), 0);
      break;
    } else if(nread == 0) {
      // 客户端连接已断开。
      closeCallback();
      break;
    }
  }
}