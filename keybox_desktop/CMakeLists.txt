cmake_minimum_required(VERSION 3.26)
project(keybox)

set(CMAKE_CXX_STANDARD 17)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt5_wrap_cpp(MOC_SOURCES MainWindow.h)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})


add_executable(keyboxd main.cpp
        MainWindow.cpp
        MainWindow.h
        ${MOC_SOURCES}
        resouces.qrc
        EntryDlg.cpp
        EntryDlg.h
        PasswordBox.cpp
        PasswordBox.h
        CKBModel.cpp
        CKBModel.h
        PrimaryPasswordDlg.cpp
        PrimaryPasswordDlg.h
)

target_link_libraries(
        keyboxd
        Qt5::Core
        Qt5::Widgets
        utilities
)