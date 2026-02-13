cmake_minimum_required(VERSION 3.10)

include_directories(
    ../include
)
include(GoogleTest)

add_executable(
    all_test
    hello_test.cpp
    key_manager_demo.cpp
)

target_link_libraries(
    all_test
    PRIVATE learn_cpp
    spdlog
    crypto
    fmt
    GTest::gtest_main
)

# add_test(
#     NAME key_manager_demo
#     COMMAND key_manager_demo
# )
gtest_discover_tests(all_test key_manager_demo)

# add_executable(
#     storage_test
#     storage_test.cpp
#     ../src/Storage.cpp
#     ../src/KeyManager.cpp
#     ../src/Config.cpp
# )

# target_link_libraries(
#     storage_test
#     spdlog
#     crypto
#     sqlite3
# )

# add_test(
#     NAME storage_test
#     COMMAND storage_test
# )