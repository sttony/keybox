cmake_minimum_required(VERSION 3.26)
project(keybox)

set(CMAKE_CXX_STANDARD 17)

set(HEADER_FILES
        win32_handler.h
        CipherEngine.h
)

IF(WIN32)
    set(SOURCE_FILES
            CipherEngine_win.cpp
    )
ENDIF (WIN32)
add_library(utilities STATIC ${SOURCE_FILES} ${HEADER_FILES})

IF(WIN32)
    target_link_libraries(utilities
            PRIVATE Bcrypt)
ENDIF (WIN32)
