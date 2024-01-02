cmake_minimum_required(VERSION 3.26)
project(keybox)

set(CMAKE_CXX_STANDARD 17)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
qt5_wrap_cpp(MOC_SOURCES MainWindow.h)
include_directories(${Qt5Widgets_INCLUDE_DIRS})


add_executable(keyboxd main.cpp
        MainWindow.cpp
        MainWindow.h
        ${MOC_SOURCES}
)

target_link_libraries(
        keyboxd
        Qt5::Core
        Qt5::Widgets
)