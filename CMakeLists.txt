cmake_minimum_required(VERSION 3.26)
project(keybox)

set(CMAKE_CXX_STANDARD 17)

add_executable(keybox main.cpp)

include_directories(utilities)
add_subdirectory(utilities)


target_link_libraries(keybox utilities)
