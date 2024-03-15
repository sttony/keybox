cmake_minimum_required(VERSION 3.26)
project(keybox)

set(CMAKE_CXX_STANDARD 17)

IF (WIN32)
    set(SOURCE_FILES
            main.cpp
            help.h
            help_win32.cpp
    )
ELSEIF (APPLE)
    set(SOURCE_FILES
            main.cpp
            help.h
            help_mac.cpp
    )
ENDIF (WIN32)

add_executable(keepassdump ${SOURCE_FILES})
target_link_libraries(keepassdump utilities)
target_link_libraries(keepassdump ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} pugixml)