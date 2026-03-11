#include "base/thread_pool.h"

#include <unistd.h>
#include <string>

void
show(int no, const std::string& name) {
  printf("小哥哥们好，我是第%d号超级女生%s。\n", no, name.c_str());
}

void
test() {
  printf("我有一只小小鸟。\n");
}

int
main() {
  ThreadPool threadpool(3);

  std::string name = "西施";
  threadpool.addTask(std::bind(show, 8, name));
  sleep(1);

  threadpool.addTask(std::bind(test));
  sleep(1);

  threadpool.addTask(std::bind([] { printf("我是一只傻傻鸟。\n"); }));
  sleep(1);

  sleep(200);
}