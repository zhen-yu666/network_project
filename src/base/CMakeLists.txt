set(SOURCES
  inet_address.cpp
)

add_library(base
  STATIC
    ${SOURCES}
)

target_include_directories(base
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)