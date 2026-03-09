# tests/test_base/CMakeLists.txt

add_executable(test_base
  test_channel.cpp
)

target_link_libraries(test_base
  PRIVATE
    base
    # project_defines
)

target_include_directories(test_base
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

add_test(NAME TestBase COMMAND test_base)