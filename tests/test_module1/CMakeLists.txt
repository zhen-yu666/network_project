add_executable(test_module2
  test_event_loop.cpp
)

target_link_directories(test_module2 PRIVATE module2)

# 注册测试
add_test(NAME TestModule2 COMMADN test_module2)