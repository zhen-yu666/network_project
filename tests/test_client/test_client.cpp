/**
 * @file test_client.cpp
 * @brief 网络通讯的客户端程序。
 * @author  ()
 * @date 2026-03-07
 * 
 * @copyright Copyright (c) 2026
 * 
*/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int
main(int argc, char* argv[]) {
  if(argc != 3) {
    printf("usage:./client ip port\n");
    return -1;
  }

  int sockfd;
  struct sockaddr_in servaddr;
  char buf[1024];

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("socket() failed.\n");
    return -1;
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  servaddr.sin_addr.s_addr = inet_addr(argv[1]);

  if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
    printf("connect(%s:%s) failed.\n", argv[1], argv[2]);
    close(sockfd);
    return -1;
  }

  printf("connect ok.\n");
  // printf("开始时间：%d",time(0));

  for(int ii = 0; ii < 100; ii++) {
    // 从命令行输入内容。
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "这是第%d个超级女生。", ii);

    char tmp_buf[1024];
    memset(tmp_buf, 0, sizeof(tmp_buf));
    // 报文大小
    int len = strlen(buf);
    // 加头部长度
    memcpy(tmp_buf, &len, 4);
    // 拼接内容
    memcpy(tmp_buf + 4, buf, len);

    // 把命令行输入的内容发送给服务端。
    send(sockfd, tmp_buf, len + 4, 0);
  }

  for(int i = 0; i < 100; ++i) {
    int len = 0;
    // 先读取长度
    recv(sockfd, &len, 4, 0);

    memset(buf, 0, sizeof(buf));
    // 再读取内容
    recv(sockfd, buf, len, 0);
    printf("recv:%s\n", buf);
  }

  // printf("结束时间：%d",time(0));
}