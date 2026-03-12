#pragma once

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

class Timestamp {
public:
  // 使用 steady_clock 保证单调递增，适合测量时间间隔
  using Clock = std::chrono::steady_clock;
  using TimePoint = Clock::time_point;

private:
  TimePoint time_point_;

public:
  Timestamp() : time_point_(Clock::now()) {}
  explicit Timestamp(TimePoint tp) : time_point_(tp) {}

  static Timestamp now() { return Timestamp(Clock::now()); }

  // 返回从该时间点到现在的秒数（浮点数）
  double elapsedSeconds() const {
    auto now = Clock::now();
    return std::chrono::duration<double>(now - time_point_).count();
  }

  // 判断是否超过指定的超时时间（秒）
  bool isTimeout(double seconds) const { return elapsedSeconds() >= seconds; }

  // 获取内部时间点（用于比较）
  const TimePoint& get() const { return time_point_; }

  // 转换为可读字符串（仅用于日志）
  std::string toString() const {
    auto sys_now = std::chrono::system_clock::now();
    auto steady_now = Clock::now();
    auto sys_tp = sys_now - (steady_now - time_point_);
    auto tt = std::chrono::system_clock::to_time_t(sys_tp);
    std::tm tm = *std::localtime(&tt);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
  }
};

#endif