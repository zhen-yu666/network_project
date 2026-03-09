# src/net/

set(SOURCES
  event_loop.cpp
  tcp_server.cpp
  acceptor.cpp
  connection.cpp
)

add_library(net
  STATIC
    ${SOURCES}
)

target_link_libraries(net
  PRIVATE
    base
    project_defines
)

target_include_directories(net
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)