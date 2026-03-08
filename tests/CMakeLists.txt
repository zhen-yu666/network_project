option(BUILD_TEST_BASE "Build test_base" ON)
option(BUILD_TEST_CLIENT "Build test_client" ON)

if(BUILD_TEST_BASE)
  add_subdirectory(test_base)
endif()

if(BUILD_TEST_CLIENT)
  add_subdirectory(test_client)
endif()