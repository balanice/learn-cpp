cmake_minimum_required(VERSION 3.5)
project(learn-cpp VERSION 0.1.0 LANGUAGES C CXX)

include_directories(include)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    learn-cpp
    main.cpp
    SingletonData.cpp
    database.cpp
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_FLAGS -g)
include(CPack)

target_link_libraries(learn-cpp curl sqlite3)
