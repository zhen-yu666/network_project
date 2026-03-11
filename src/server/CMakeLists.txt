# src/server/

set(SOURCES
  echo_server.cpp
)

add_library(server
  STATIC
    ${SOURCES}
)

target_link_libraries(server
  PRIVATE
    net
)

target_include_directories(server
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/incldue>
)