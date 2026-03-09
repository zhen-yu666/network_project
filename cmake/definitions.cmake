# cmake/

# 创建一个接口库，用于统一管理宏定义
add_library(project_defines INTERFACE)

# 可以根据条件添加宏
option(ENABLE_TEST_MACROS "Enable test-specific macros" OFF)
if(ENABLE_TEST_MACROS)
    target_compile_definitions(project_defines INTERFACE
      # EPOLL_DEBUG
      # CHANNEL_DEBUG
      CHANNEL_DEBUG
      TCP_SERVER_DEBUG
      # ACCEPTOR_DEBUG
    )
endif()

# 总是添加一些全局宏
# target_compile_definitions(project_defines INTERFACE
#     _GLIBCXX_USE_CXX11_ABI=1   # 举例
# )