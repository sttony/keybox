cmake_minimum_required(VERSION 3.26)
project(keybox)

set(CMAKE_CXX_STANDARD 17)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt5_wrap_cpp(MOC_SOURCES MainWindow.h)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED unit_test_framework filesystem)

find_package(OpenSSL REQUIRED)


add_executable(keyboxd main.cpp
        MainWindow.cpp
        MainWindow.h
        ${MOC_SOURCES}
        resouces.qrc
        CPwdEntryDlg.cpp
        CPwdEntryDlg.h
        CPasswordBox.cpp
        CPasswordBox.h
        CKBModel.cpp
        CKBModel.h
        CPrimaryPasswordDlg.cpp
        CPrimaryPasswordDlg.h
        CPwdEntryTableView.cpp
        CPwdEntryTableView.h
        CPwdGeneratorDlg.cpp
        CPwdGeneratorDlg.h
        CSettings.cpp
        CSettings.h
        CPwdGroupListView.cpp
        CPwdGroupListView.h
        CPwdGroupDlg.cpp
        CPwdGroupDlg.h
        CSyncSettingDlg.h
        CSyncSettingDlg.cpp
        CSyncDlg.cpp
        CSyncDlg.h
        CNewClientDlg.cpp
        CNewClientDlg.h
)

target_link_libraries(
        keyboxd
        Qt5::Core
        Qt5::Widgets
        utilities
        Boost::filesystem
)