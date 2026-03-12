#include "net/event_loop.h"

#include "base/channel.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <cstdlib>

void
EventLoop::init() {
  // 创建 eventfd，初始计数器为0，非阻塞模式 + 执行 exec 时自动关闭
  wakeup_fd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

  if(wakeup_fd_ < 0)
    exit(-1);

  // 为 wakeupFd_ 创建 Channel，并设置读回调为 handleRead
  wakeup_channel_ = std::make_unique<Channel>(this, wakeup_fd_);

  wakeup_channel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
  // 让 epoll 监听 wakeupFd_ 的读事件
  wakeup_channel_->enableReading();
}

void
EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = write(wakeup_fd_, &one, sizeof(one));
  if(n != sizeof(one)) {
    // 写入失败可忽略（可能被信号中断），生产环境可记录日志
  }
}

void
EventLoop::handleRead() {
  uint64_t one;
  ssize_t n = read(wakeup_fd_, &one, sizeof(one));
  if(n != sizeof(one)) {
    // 读取失败可忽略
  }
  // 读走数据后，eventfd 计数器清零，下次可以再次被唤醒
}

void
EventLoop::doPendingFunctors() {
  std::vector<std::function<void()>> functors;
  // 标记正在执行，防止在内部调用 queueInLoop 时误以为不需要唤醒
  calling_pending_functors_ = true;

  {
    // 消费者：加锁将 pending_functors_ 中的所有任务交换到局部队列，然后立即释放
    std::lock_guard<std::mutex> lock(mtx_);
    functors.swap(pending_functors_);
  }

  // 在局部队列上执行所有任务（此时已无锁，不会阻塞其他生产者）
  for(const auto& functor : functors) {
    functor();
  }

  calling_pending_functors_ = false;
}

EventLoop::~EventLoop() {
  delete ep_;
  ep_ = nullptr;

  close(wakeup_fd_);
}

void
EventLoop::run() {
  while(true) {
    // 存储epoll的rdlist的所有事件
    std::vector<Channel*> channels = std::move(ep_->loop(10 * 1000));

    if(channels.empty()) {
      // epoll_wait 超时，调用用户设置的回调
      epoll_timeout_callback_(this);
    } else {
      // 处理所有就绪的 IO 事件
      for(auto& ch : channels) {
        ch->handLevent();
      }
    }
    // 每次事件循环的末尾，执行所有跨线程投递的任务（pending functors）
    doPendingFunctors();
  }
}

void
EventLoop::updateChannel(Channel* ch) {
  ep_->updateChannel(ch);
}

void
EventLoop::removeChannel(Channel* ch) {
  ep_->removeChannel(ch);
}


void
EventLoop::runInLoop(std::function<void()> cb) {
  if(isInLoopThread()) {
    // 当前线程就是本 loop 线程，直接执行（无锁、无唤醒、高效）
    cb();
  } else {
    // 当前线程不是本 loop 线程，通过 queueInLoop 跨线程投递
    queueInLoop(std::move(cb));
  }
}


void
EventLoop::queueInLoop(std::function<void()> cb) {
  {
    // 生产者：加锁将任务放入队列
    std::lock_guard<std::mutex> lock(mtx_);
    pending_functors_.emplace_back(std::move(cb));
  }

  // 判断是否需要唤醒目标 loop 线程：
  // 1. 如果当前线程不是目标 loop 线程，则必须唤醒（因为目标线程可能正阻塞在 epoll_wait）
  // 2. 如果当前线程就是目标 loop 线程，但正在执行 pending functors（即 doPendingFunctors 内部又调用了 queueInLoop），
  //    也需要唤醒，否则新加入的任务可能被延迟到下一次事件循环才能执行。
  if(!isInLoopThread() || calling_pending_functors_) {
    wakeup();
  }
}