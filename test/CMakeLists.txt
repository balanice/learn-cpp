cmake_minimum_required(VERSION 3.10)

include_directories(
    ../include
)
include(GoogleTest)

add_executable(
    all_test
    hello_test.cpp
    key_manager_demo.cpp
    key_manager_test.cpp
    storage_test.cpp
    config_test.cpp
    # logging_test.cpp
    leetcode_test.cpp
)

target_link_libraries(
    all_test
    PRIVATE learn_cpp
    spdlog
    crypto
    fmt
    GTest::gtest_main
)

gtest_discover_tests(all_test)