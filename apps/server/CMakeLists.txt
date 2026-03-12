# apps/server

add_executable(echoserver server.cpp)

target_link_libraries(echoserver
  PRIVATE
    base
    server
    net
)

target_include_directories(echoserver
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)