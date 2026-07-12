cmake_minimum_required(VERSION 3.10)
project(learn_cpp VERSION 0.1.0 LANGUAGES C CXX)

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
  DOWNLOAD_EXTRACT_TIMESTAMP NEW
)
# 禁止GTest自带的测试
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(
    learn_cpp STATIC
    src/database.cpp
    src/StatementWrapper.cpp
    src/MyCrypt2.cpp
    src/KeyManager.cpp
    src/DateSizeRotatingSink.cpp
    src/curl_test2.cpp
    src/Utils.cpp
    src/MessageWorker.cpp
    src/Config.cpp
    src/Storage.cpp
    src/LeetCode.cpp
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_FLAGS -g)
include(CPack)

target_link_libraries(
    learn_cpp
    curl
    sqlite3
    crypto
    spdlog
    fmt
)

# 暴露头文件路径，方便测试程序包含
target_include_directories(learn_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})