set(SOURCES
  inet_address.cpp
)

add_library(utility
  STATIC
    ${SOURCES}
)

target_include_directories(utility
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)