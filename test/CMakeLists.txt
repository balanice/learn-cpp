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

add_executable(
    key_manager_demo
    key_manager_demo.cpp
    ../src/KeyManager.cpp
)

target_link_libraries(
    key_manager_demo
    spdlog
    crypto
    fmt
)

add_test(
    NAME key_manager_demo
    COMMAND key_manager_demo
)

add_executable(
    storage_test
    storage_test.cpp
    ../src/Storage.cpp
    ../src/KeyManager.cpp
    ../src/Config.cpp
)

target_link_libraries(
    storage_test
    spdlog
    crypto
    sqlite3
)

add_test(
    NAME storage_test
    COMMAND storage_test
)