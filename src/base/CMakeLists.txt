# src/base/

set(SOURCES
  inet_address.cpp
  socket.cpp
  epoll.cpp
  channel.cpp
)

add_library(base
  STATIC
    ${SOURCES}
)

target_link_libraries(base
  PRIVATE
    # project_defines
)

target_include_directories(base
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)