# 收集源文件
set(SOURCES
  add.cpp
  sub.cpp
)

# 创建库（静态/动态可选）
# 动态库SHARED
add_library(module1 STATIC ${SOURCES})

# 指定头文件目录
target_include_directories(module1
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        # $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)