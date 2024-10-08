cmake_minimum_required(VERSION 3.5)
project(learn-cpp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_subdirectory(test)

include_directories(
    ./include
    )

include(CTest)
enable_testing()

add_executable(
    learn-cpp
    main.cpp
    SingletonData.cpp
    database.cpp
    StatementWrapper.cpp
    Crypt.cpp
    MyCrypt2.cpp
    src/AutoPtrTest.cpp
    src/curl_test2.cpp
    src/JsonTest.cpp
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
    cjson
)
