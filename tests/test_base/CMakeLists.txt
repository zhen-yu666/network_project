# tests/test_base/

add_executable(test_base
  test_thread_pool.cpp
)

target_link_libraries(test_base
  PRIVATE
    base
)

target_include_directories(test_base
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

add_test(NAME TestBase COMMAND test_base)