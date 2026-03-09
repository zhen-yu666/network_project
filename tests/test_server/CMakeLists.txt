# tests/test_server/

add_executable(test_server
  test_event_loop.cpp
)

target_link_libraries(test_server
  PRIVATE
    base
    net
    # project_defines
)

target_include_directories(test_server
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

add_test(NAME TestServer COMMAND test_server)