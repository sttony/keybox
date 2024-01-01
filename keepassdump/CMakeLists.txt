cmake_minimum_required(VERSION 3.26)
project(keybox)

set(CMAKE_CXX_STANDARD 17)

add_executable(keepassdump main.cpp
        help.h
        help_win32.cpp)
target_link_libraries(keepassdump utilities)
target_link_libraries (keepassdump  ${Boost_LIBRARIES}  ${ZLIB_LIBRARIES}  pugixml)