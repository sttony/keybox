cmake_minimum_required(VERSION 3.26)
project(keybox)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)
set (Boost_USE_STATIC_LIBS ON)
find_package (Boost )


include_directories (${Boost_INCLUDE_DIRS})
set(HEADER_FILES
        win32_handler.h
        CipherEngine.h
        KeepassReader.h
        CGZcompressor.h
        Salsa20Cipher.h
        RandomGenerator.h
        Base64Coder.cpp
        Base64Coder.h
        CKBFile.h
        CPwdEntry.h
        CMaskedPwd.h
)

IF(WIN32)
    set(SOURCE_FILES
            CipherEngine_win.cpp
            KeepassReader.cpp
            CGZcompressor.cpp
            Salsa20Cipher.cpp
            RandomGenerator.cpp
            Base64Coder.cpp
            CKBFile.cpp
            CPwdEntry.cpp
            CMaskedPwd.cpp
    )
ELSEIF (APPLE)
    set(SOURCE_FILES
            CipherEngine_mac.cpp
            KeepassReader.cpp
            CGZcompressor.cpp
            Salsa20Cipher.cpp
            RandomGenerator.cpp
            Base64Coder.cpp
            CPwdEntry.cpp
            CMaskedPwd.cpp
    )
ENDIF (WIN32)

add_library(utilities STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(utilities PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(utilities PRIVATE ${ZLIB_LIBRARIES})

IF(WIN32)
    target_link_libraries(utilities
            PRIVATE Bcrypt)
ENDIF (WIN32)
