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

      while(true) {
        // 1 检查是否至少有 4 字节头部
        if(input_buffer_.readableBytes() < sizeof(uint32_t))
          break;
        // 2 查看头部长度（不移动指针）
        uint32_t len = input_buffer_.peekInt32();
        // 3 检查是否包含完整报文（头部 + 内容）
        if(input_buffer_.readableBytes() < len + sizeof(uint32_t))
          break;
        // 4 取出完整报文
        // 4.1 跳过头部
        input_buffer_.retrieve(sizeof(uint32_t));
        // 4.2 提取内容
        std::string message(input_buffer_.peek(), len);
        // 4.3 跳过内容
        input_buffer_.retrieve(len);

        // 5 处理报文
        printf("message (eventfd=%d): %s\n", fd(), message.c_str());

        onMsgCallback_(this, std::move(message));
      }
      break;
    } else if(nread == 0) {
      // 客户端连接已断开。
      closeCallback();
      break;
    }
  }
}