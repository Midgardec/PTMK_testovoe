cmake_minimum_required(VERSION 3.16)
project(PTMK_testovoe)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/SQLiteCpp-master)
include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/SQLiteCpp-master/include
)
add_executable(PTMK_testovoe myApp.cpp)
target_link_libraries(PTMK_testovoe
        SQLiteCpp
        sqlite3
)
