cmake_minimum_required(VERSION 3.26)
project(keybox)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)
find_package(pugixml REQUIRED)

include(${CMAKE_TOOLCHAIN_FILE})

#set(Boost_DEBUG ON)
set (Boost_USE_STATIC_LIBS ON)

include_directories (${Boost_INCLUDE_DIRS})


add_executable(keybox main.cpp)

include_directories(utilities)
add_subdirectory(utilities)

target_include_directories(keybox PRIVATE ${ZLIB_INCLUDE_DIRS})


target_link_libraries(keybox utilities)
target_link_libraries (keybox  ${Boost_LIBRARIES}  ${ZLIB_LIBRARIES}  pugixml)

add_subdirectory(boost_tests)
