cmake_minimum_required(VERSION 3.10)

include_directories(
    ../include
)

add_executable(
    hello_test
    hello_test.cpp
    ../src/Utils.cpp
)

target_link_libraries(
    hello_test
    spdlog
    fmt
)

add_test(
    NAME hello_test
    COMMAND hello_test
)