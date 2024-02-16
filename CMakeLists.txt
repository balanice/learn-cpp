cmake_minimum_required(VERSION 3.5)
project(learn-cpp VERSION 0.1.0 LANGUAGES C CXX)

include_directories(include)

include(CTest)
enable_testing()

add_executable(
    learn-cpp
    main.cpp
    SingletonData.cpp
    database.cpp
    )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(learn-cpp curl sqlite3)
