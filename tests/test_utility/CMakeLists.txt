add_executable(test_utility
  test_inet_address.cpp
)

target_link_libraries(test_utility
  PRIVATE
    utility
)

target_include_directories(test_utility
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

add_test(NAME TestUtility COMMAND test_utility)