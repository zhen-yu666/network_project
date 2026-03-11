# tests/

option(BUILD_TEST_SERVER "Build test_server" ON)
option(BUILD_TEST_CLIENT "Build test_client" ON)
option(BUILD_TEST_BASE "Build test_base" ON)

if(BUILD_TEST_SERVER)
  add_subdirectory(test_server)
endif()

if(BUILD_TEST_CLIENT)
  add_subdirectory(test_client)
endif()

if(BUILD_TEST_BASE)
  add_subdirectory(test_base)
endif()