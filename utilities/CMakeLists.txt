cmake_minimum_required(VERSION 3.26)
project(keybox)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost)


include_directories(${Boost_INCLUDE_DIRS})
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
)

IF (WIN32)
    set(SOURCE_FILES
            CipherEngine_win.cpp
            KeepassReader.cpp
            CGZcompressor.cpp
            Salsa20Cipher.cpp
            CRandomGenerator.cpp
            Base64Coder.cpp
            CKBFileHeader.cpp
            CKBFile.cpp
            CPwdEntry.cpp
            CMaskedBlob.cpp
            InitGlobalRG_win.cpp
            CPasswordGenerator.cpp
            CPwdGroup.cpp
    )
ELSEIF (APPLE)
    set(SOURCE_FILES
            CipherEngine_mac.cpp
            KeepassReader.cpp
            CGZcompressor.cpp
            Salsa20Cipher.cpp
            CRandomGenerator.cpp
            Base64Coder.cpp
            CPwdEntry.cpp
            CMaskedBlob.cpp
            CKBFileHeader.cpp
            CKBFile.cpp
            InitGlobalRG_mac.cpp
            CPasswordGenerator.cpp
            CPwdGroup.cpp
    )
ELSEIF (LINUX)
    set(SOURCE_FILES
            CipherEngine_linux.cpp
            KeepassReader.cpp
            CGZcompressor.cpp
            Salsa20Cipher.cpp
            CRandomGenerator.cpp
            Base64Coder.cpp
            CPwdEntry.cpp
            CMaskedBlob.cpp
            InitGlobalRG_linux.cpp
            CKBFileHeader.cpp
            CKBFile.cpp
            CPasswordGenerator.cpp
            CPwdGroup.cpp

    )
ENDIF (WIN32)

IF(LINUX)
    find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIR})

ENDIF (LINUX)


add_library(utilities STATIC ${SOURCE_FILES} ${HEADER_FILES})

IF (LINUX)
    target_link_libraries(utilities PRIVATE ${OPENSSL_LIBRARIES})
ENDIF (LINUX)

target_include_directories(utilities PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(utilities PRIVATE ${ZLIB_LIBRARIES})

IF (WIN32)
    target_link_libraries(utilities
            PRIVATE Bcrypt)
ENDIF (WIN32)
