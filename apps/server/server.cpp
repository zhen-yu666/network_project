/**
 * @file server.cpp
 * @brief 回显（EchoServer）服务器。
 * @author  ()
 * @date 2026-03-12
 * 
 * @copyright Copyright (c) 2026
 * 
*/

#include <signal.h>
#include "server/echo_server.h"

// 1、设置2和15的信号。
// 2、在信号处理函数中停止主从事件循环和工作线程。
// 3、服务程序主动退出。

EchoServer* echoserver;

void
stop(int sig)  // 信号2和15的处理函数，功能是停止服务程序。
{
  printf("sig=%d\n", sig);
  // 调用EchoServer::Stop()停止服务。
  echoserver->stop();
  printf("echoserver已停止。\n");
  delete echoserver;
  printf("delete echoserver。\n");
  exit(0);
}

int
main(int argc, char* argv[]) {
  if(argc != 3) {
    printf("usage: ./bin/server ip port\n");
    printf("example: ./bin/server 10.6.0.14 8080\n\n");
    return -1;
  }

  signal(SIGTERM, stop);  // 信号15，系统kill或killall命令默认发送的信号。
  signal(SIGINT, stop);  // 信号2，按Ctrl+C发送的信号。

  echoserver = new EchoServer(argv[1], atoi(argv[2]), 3, 2);
  echoserver->start();

  return 0;
}
