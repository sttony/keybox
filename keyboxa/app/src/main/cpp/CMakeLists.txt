cmake_minimum_required(VERSION 3.22.1)
project("keyboxa")

# The core utilities library
add_subdirectory(../../../../utilities ${CMAKE_CURRENT_BINARY_DIR}/utilities_build)

add_library(keyboxa SHARED
        native-lib.cpp
)

target_link_libraries(keyboxa
        utilities
        log
)
