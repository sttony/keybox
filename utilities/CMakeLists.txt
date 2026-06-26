cmake_minimum_required(VERSION 3.22.1)
project(keybox)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)
if(APPLE AND IOS)
    set(OPENSSL_USE_STATIC_LIBS TRUE)
    set(CURL_USE_STATIC_LIBS TRUE)
    # Allow CMake to search for libraries/includes outside of the iOS SDK (Sysroot)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
elseif(ANDROID)
    set(KEYBOX_ANDROID_DEPS_DIR "${CMAKE_CURRENT_LIST_DIR}/../android_deps")
    set(Boost_INCLUDE_DIRS "${KEYBOX_ANDROID_DEPS_DIR}")
    set(Boost_FOUND TRUE)
    set(OPENSSL_ROOT_DIR "${KEYBOX_ANDROID_DEPS_DIR}/build/openssl/${ANDROID_ABI}")
    set(OPENSSL_USE_STATIC_LIBS TRUE)
    set(CURL_ROOT "${KEYBOX_ANDROID_DEPS_DIR}/build/curl/${ANDROID_ABI}")
    set(CURL_USE_STATIC_LIBS TRUE)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
endif()

if(ANDROID)
    # Boost is header-only, no find_package needed
else()
    set(Boost_USE_STATIC_LIBS ON)
    find_package(Boost REQUIRED)
endif()
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})

set(HEADER_FILES
        win32_handler.h
        CipherEngine.h
        KeepassReader.h
        CGZcompressor.h
        Salsa20Cipher.h
        CRandomGenerator.h
        Base64Coder.cpp
        Base64Coder.h
        CKBFile.h
        CKBFileHeader.h
        CPwdEntry.h
        CMaskedBlob.h
        InitGlobalRG.h
        CPasswordGenerator.h
        CPwdGroup.h
        CRequest.h
)
set(COMMON_SOURCE_FILES
        KeepassReader.cpp
        CGZcompressor.cpp
        Salsa20Cipher.cpp
        CRandomGenerator.cpp
        Base64Coder.cpp
        CKBFileHeader.cpp
        CKBFile.cpp
        CPwdEntry.cpp
        CMaskedBlob.cpp
        CPasswordGenerator.cpp
        CPwdGroup.cpp
        CAsymmetricKeyPair.cpp
        CRequest.cpp
)

IF (WIN32)
    set(SOURCE_FILES
            CipherEngine_win.cpp
            InitGlobalRG_win.cpp
            ${COMMON_SOURCE_FILES}
    )
ELSEIF (APPLE)
    set(SOURCE_FILES
            CipherEngine_mac.cpp
            InitGlobalRG_mac.cpp
            ${COMMON_SOURCE_FILES}
    )
ELSEIF (LINUX OR ANDROID)
    set(SOURCE_FILES
            CipherEngine_linux.cpp
            InitGlobalRG_linux.cpp
            ${COMMON_SOURCE_FILES}
    )
ENDIF (WIN32)



add_library(utilities STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(utilities
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
        PRIVATE
        ${ZLIB_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
)

target_link_libraries(utilities PRIVATE ${OPENSSL_LIBRARIES})

target_link_libraries(utilities PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(utilities PRIVATE ${CURL_LIBRARIES})

IF (WIN32)
    target_link_libraries(utilities
            PRIVATE Bcrypt)
ELSEIF (APPLE)
    target_link_libraries(utilities
            PRIVATE "-framework Security")
ENDIF (WIN32)
