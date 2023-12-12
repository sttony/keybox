cmake_minimum_required(VERSION 3.26)
project(keybox)
set (Boost_USE_STATIC_LIBS ON)
set(CMAKE_CXX_STANDARD 17)
#set(Boost_DEBUG ON)
find_package (Boost REQUIRED unit_test_framework)


include_directories (${Boost_INCLUDE_DIRS})

add_executable (Boost_Tests_run CipherTest.cpp
        ../utilities/win32_handler.h
        KdbxReader.cpp)
target_link_libraries (Boost_Tests_run
        utilities
)
target_link_libraries (Boost_Tests_run
        ${Boost_LIBRARIES}
)