cmake_minimum_required(VERSION 3.5)
project(learn-cpp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(test)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.82.0 REQUIRED COMPONENTS log PATHS ~/projects/boost_1_82_0/build/lib/cmake)

include_directories(
    ./include
    ${Boost_INCLUDE_DIRS}
    )
link_directories(${Boost_LIBRARY_DIRS})

include(CTest)
enable_testing()

enable_testing()

add_executable(
    learn-cpp
    main.cpp
    SingletonData.cpp
    database.cpp
    StatementWrapper.cpp
    Crypt.cpp
    MyCrypt2.cpp
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
    ${Boost_LIBRARIES}
)
