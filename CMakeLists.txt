cmake_minimum_required(VERSION 3.10)
project(learn-cpp VERSION 0.1.0 LANGUAGES C CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ./include
    /usr/include/fmt
    )

# 启用CTest
enable_testing()

# 添加测试子目录
add_subdirectory(test)

# gtest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
)
# 禁止GTest自带的测试
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
    learn-cpp
    main.cpp
    SingletonData.cpp
    src/database.cpp
    src/StatementWrapper.cpp
    Crypt.cpp
    MyCrypt2.cpp
    src/KeyManager.cpp
    src/curl_test2.cpp
    src/JsonTest.cpp
    src/Utils.cpp
    src/MessageWorker.cpp
    src/Config.cpp
    src/Storage.cpp
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_FLAGS -g)
include(CPack)

target_link_libraries(
    learn-cpp
    curl
    sqlite3
    crypto
    spdlog
    fmt
)
