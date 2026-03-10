#pragma once

#ifndef BUFFER_H
#define BUFFER_H

#include <cstddef>
#include <vector>

class Buffer {
private:
  std::vector<char> buffer_;
  // 存储了从网络接收到或准备发送的实际有效数据。
  // 从 @readerIndex_ 开始，到 writerIndex_ 结束。
  size_t reader_index_;
  // 位于可读数据之后，用于追加新的数据。
  // 从 writerIndex_ 开始，到缓冲区末尾结束。
  size_t writer_index_;

public:
  // 位于缓冲区的最前端。
  // 它的一个妙用是在已有数据前方便地添加协议头（如消息长度），而无需移动现有数据。
  // 字节数
  static const size_t CheapPrepend = 8;
  // 默认初始容量
  static const size_t InitialSize = 1024;

private:
  // 起始位置
  char* begin() { return &*buffer_.begin(); }
  const char* begin() const { return &*buffer_.cbegin(); }

  // 腾出或扩容至足够写出 len 字节
  void makeSpace(size_t len);

public:
  explicit Buffer(size_t size = InitialSize);

  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

  Buffer(Buffer&& other) noexcept;
  Buffer& operator=(Buffer&& other) noexcept;

  ~Buffer() = default;

  // ---------- 原有接口（兼容旧代码）----------

  // 追加数据
  void append(const char* data, size_t len);

  // 可读数据大小
  size_t size() const { return readableBytes(); }

  // 可读数据起始指针
  const char* data() const { return peek(); }

  // 清空缓冲区（保留内存）
  void clear() { retrieveAll(); }

  // ---------- 新增高效接口 ----------

  // 可读数据长度
  size_t readableBytes() const { return writer_index_ - reader_index_; }

  // 可写空间长度
  size_t writableBytes() const { return buffer_.size() - writer_index_; }

  // 可预置空间长度
  size_t prependableBytes() const { return reader_index_; }

  // 可读数据起始位置
  const char* peek() const { return begin() + reader_index_; }

  // 可以写数据的起始位置
  char* beginWrite() { return begin() + writer_index_; }
  const char* beginWrite() const { return begin() + writer_index_; }

  // 读取 len 字节后移动读指针
  void retrieve(size_t len);

  // 将读写指针都重置到预留空间之后，表示缓冲区已空。
  // 之前已读的数据空间（0 到 readerIndex_）被逻辑上回收，成为新的 prependable 空间。
  void retrieveAll() {
    reader_index_ = CheapPrepend;
    writer_index_ = CheapPrepend;
  }

  // 确保至少有 len 字节可写空间
  void ensureWritableBytes(size_t len);

  // 写完数据后移动写指针
  void hasWritten(size_t len) { writer_index_ += len; }

  size_t capacity() const { return buffer_.capacity(); }
};

#endif