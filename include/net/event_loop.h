#pragma once

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <net/connection.h>
#include "base/epoll.h"

#include <sys/eventfd.h>
#include <functional>
#include <map>
#include <mutex>
#include <thread>

enum class LoopType : bool { MainLoop, SubLoop };

class Epoll;
class Channel;

class EventLoop {
private:
  // 每个事件循环只有一个Epoll。
  std::unique_ptr<Epoll> ep_;
  // epoll_wait()超时的回调函数。
  std::function<void(EventLoop*)> epoll_timeout_callback_;

  // 记录本 EventLoop 所属的线程 ID
  std::thread::id thread_id_;
  // eventfd 文件描述符，用于唤醒
  int64_t wakeup_fd_;
  // 包装 wakeupFd_ 的 Channel
  std::unique_ptr<Channel> wakeup_channel_;
  // 待执行的任务队列（由其他线程投递）
  std::vector<std::function<void()>> pending_functors_;
  // 保护 pendingFunctors_ 的互斥锁
  std::mutex mtx_;
  // 标记当前是否正在执行 pending functors
  bool calling_pending_functors_;

  int timer_fd_;
  std::unique_ptr<Channel> timer_channel_;

  // 本 loop 管理的所有连接
  std::map<int, SptrConnection> conns_;
  // 默认超时 80 秒
  double idle_timeout_ = 80.0;
  // 回调 TcpServer::removeConnection
  std::function<void(SptrConnection)> remove_conn_callback_;

  std::atomic_bool stop_ = false;

private:
  void init();

  // 向 wakeupFd_ 写入数据，唤醒阻塞在 epoll_wait 的 loop 线程
  void wakeup();

  // wakeupChannel_ 的读回调，用于读取 eventfd 数据，清空通知
  void handleRead();

  // 执行所有待处理的 pending functors
  void doPendingFunctors();

public:
  // 在构造函数中创建Epoll对象ep_。
  EventLoop()
      : ep_(new Epoll),
        thread_id_(std::this_thread::get_id()),
        calling_pending_functors_(false) {
    init();
  }

  ~EventLoop();

  // 运行事件循环。
  void run();

  // 终止事件循环
  void stop();

  // 更新/添加 Channel 到 epoll
  void updateChannel(Channel* ch);

  // 从 epoll 移除 Channel
  void removeChannel(Channel* ch);

  // 设置epoll_wait()超时的回调函数。
  template<typename Callback>
  void setEpollTimeoutCallback(Callback&& cb) {
    epoll_timeout_callback_ = std::forward<Callback>(cb);
  }

  // 判断当前线程是否是本 loop 线程
  bool isInLoopThread() const {
    return std::this_thread::get_id() == thread_id_;
  }

  // 如果当前是 loop 线程则直接执行 cb，否则将 cb 放入队列并唤醒 loop 线程
  void runInLoop(const std::function<void()>& cb);

  // 无论当前线程是谁，都将 cb 放入队列并唤醒 loop 线程（如果需要）
  void queueInLoop(std::function<void()> cb);

  // 设置空闲超时时间（秒）
  void setIdleTimeout(double seconds) { idle_timeout_ = seconds; }

  // 添加连接（由 TcpServer 调用）
  void addConnection(SptrConnection conn) { conns_[conn->fd()] = conn; }

  // 从本 loop 管理的连接映射中删除指定连接
  void removeConnection(const SptrConnection& conn);

  // 定时器处理函数（应被周期性调用）
  void handleTimer();

  // 设置连接超时后的回调（由 TcpServer 提供）
  template<typename Callback>
  void setRemoveConnectionCallback(const Callback& cb) {
    remove_conn_callback_ = std::move(cb);
  }
};

#endif