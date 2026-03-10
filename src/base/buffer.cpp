#include "base/buffer.h"

#include <cassert>

void
Buffer::makeSpace(size_t len) {
  if(writableBytes() + prependableBytes() < len + CheapPrepend) {
    // 总空闲空间不足以容纳 len + CheapPrepend

    // 只能扩容
    buffer_.resize(writer_index_ + len);
  } else {
    // 总空闲空间足够，通过移动数据来腾出可写空间。

    // 确保 prependableBytes 区域确实有已读空间。
    assert(CheapPrepend < readableBytes());

    size_t readable = readableBytes();
    // 将 [readerIndex_, writerIndex_) 的数据拷贝到 [kCheapPrepend, kCheapPrepend + readable)
    std::copy(begin() + reader_index_, begin() + writer_index_,
              begin() + CheapPrepend);
    // 更新读指针
    reader_index_ = CheapPrepend;
    // 更新写指针
    writer_index_ = reader_index_ + readable;

    assert(readable == readableBytes());
  }
}

Buffer::Buffer(size_t size)
    : buffer_(InitialSize + CheapPrepend),
      reader_index_(CheapPrepend),
      writer_index_(CheapPrepend) {
  assert(readableBytes() == 0);
  assert(writableBytes() == InitialSize);
}

Buffer::Buffer(Buffer&& other) noexcept
    : buffer_(std::move(other.buffer_)),
      reader_index_(other.reader_index_),
      writer_index_(other.writer_index_) {
  other.reader_index_ = 0;
  other.writer_index_ = 0;
}

Buffer&
Buffer::operator=(Buffer&& other) noexcept {
  if(this != &other) {
    buffer_ = std::move(other.buffer_);
    reader_index_ = other.reader_index_;
    writer_index_ = other.writer_index_;
    other.reader_index_ = 0;
    other.writer_index_ = 0;
  }
  return *this;
}

void
Buffer::append(const char* data, size_t len) {
  ensureWritableBytes(len);
  std::copy(data, data + len, beginWrite());
  hasWritten(len);
}

void
Buffer::retrieve(size_t len) {
  if(len < readableBytes()) {
    reader_index_ += len;
  } else {
    // 如果取出的长度等于或超过可读数据长度，则全部取出
    retrieveAll();
  }
}

void
Buffer::ensureWritableBytes(size_t len) {
  if(writableBytes() < len) {
    makeSpace(len);
  }

  assert(writableBytes() >= len);
}
