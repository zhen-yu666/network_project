option(BUILD_TEST_UTILITY "Build test_utility" ON)
option(BUILD_TEST_CLIENT "Build test_client" ON)

if(BUILD_TEST_UTILITY)
  add_subdirectory(test_utility)
endif()

if(BUILD_TEST_CLIENT)
  add_subdirectory(test_client)
endif()