add_executable(app1 main1.cpp)

target_link_libraries(app1 PRIVATE module1)

# 安装
# install(TARGETS app1 RUNTIME DESTINATION bin)