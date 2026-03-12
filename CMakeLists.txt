# /

cmake_minimum_required(VERSION 3.15)
project(network_project VERSION 1.0.0 LANGUAGES CXX)

# 生成供给clangd使用的文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 启用测试
# enable_testing()

# 包含自定义模块路径
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# 设置附加选项
include(cmake/project_settings.cmake)
include(cmake/definitions.cmake)

# 设置输出目录（便于调试）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加子目录
add_subdirectory(src)
add_subdirectory(apps)
# add_subdirectory(tests)
# add_subdirectory(examples)
# 第三方库可选
# add_subdirectory(third_party)

# 安装配置（可选）
# install(DIRECTORY include/ DESTINATION include)