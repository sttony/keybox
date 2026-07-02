cmake_minimum_required(VERSION 3.22.1)
project("keyboxa")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(KEYBOX_UTILITIES_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../utilities")
set(KEYBOX_ANDROID_OPENSSL_ROOT "" CACHE PATH "Android ABI-specific OpenSSL install root")
set(KEYBOX_ANDROID_CURL_ROOT "" CACHE PATH "Android ABI-specific libcurl install root")
option(KEYBOX_ALLOW_ANDROID_STUB_NATIVE "Build a stub JNI library when Android OpenSSL/libcurl are not configured" ON)

if(ANDROID)
    set(KEYBOX_ANDROID_DEPS_DIR "${KEYBOX_UTILITIES_DIR}/../android_deps")
    if(NOT KEYBOX_ANDROID_OPENSSL_ROOT)
        set(KEYBOX_ANDROID_OPENSSL_ROOT "${KEYBOX_ANDROID_DEPS_DIR}/build/openssl/${ANDROID_ABI}")
    endif()
    if(NOT KEYBOX_ANDROID_CURL_ROOT)
        set(KEYBOX_ANDROID_CURL_ROOT "${KEYBOX_ANDROID_DEPS_DIR}/build/curl/${ANDROID_ABI}")
    endif()

    set(OPENSSL_SSL_LIBRARY "${KEYBOX_ANDROID_OPENSSL_ROOT}/lib/libssl.a")
    set(OPENSSL_CRYPTO_LIBRARY "${KEYBOX_ANDROID_OPENSSL_ROOT}/lib/libcrypto.a")
    set(CURL_LIBRARY "${KEYBOX_ANDROID_CURL_ROOT}/lib/libcurl.a")

    if(NOT EXISTS "${OPENSSL_SSL_LIBRARY}" OR
            NOT EXISTS "${OPENSSL_CRYPTO_LIBRARY}" OR
            NOT EXISTS "${CURL_LIBRARY}")
        if(KEYBOX_ALLOW_ANDROID_STUB_NATIVE)
            message(WARNING
                "Building stub keyboxa JNI library because Android OpenSSL/libcurl are not configured. "
                "Set -DKEYBOX_ANDROID_OPENSSL_ROOT and -DKEYBOX_ANDROID_CURL_ROOT to build the full native backend.")

            add_library(keyboxa SHARED native-stub.cpp)
            target_link_libraries(keyboxa log)
            return()
        else()
            message(FATAL_ERROR
                "Android native build requires ABI-specific OpenSSL and libcurl. "
                "Set -DKEYBOX_ANDROID_OPENSSL_ROOT=/path/to/android/openssl "
                "and -DKEYBOX_ANDROID_CURL_ROOT=/path/to/android/curl. "
                "Do not use Homebrew/macOS libraries for Android builds.")
        endif()
    endif()

    find_library(ZLIB_LIBRARY z REQUIRED)

    add_library(utilities STATIC
            "${KEYBOX_UTILITIES_DIR}/KeepassReader.cpp"
            "${KEYBOX_UTILITIES_DIR}/CGZcompressor.cpp"
            "${KEYBOX_UTILITIES_DIR}/Salsa20Cipher.cpp"
            "${KEYBOX_UTILITIES_DIR}/CRandomGenerator.cpp"
            "${KEYBOX_UTILITIES_DIR}/Base64Coder.cpp"
            "${KEYBOX_UTILITIES_DIR}/CKBFileHeader.cpp"
            "${KEYBOX_UTILITIES_DIR}/CKBFile.cpp"
            "${KEYBOX_UTILITIES_DIR}/CPwdEntry.cpp"
            "${KEYBOX_UTILITIES_DIR}/CMaskedBlob.cpp"
            "${KEYBOX_UTILITIES_DIR}/CPasswordGenerator.cpp"
            "${KEYBOX_UTILITIES_DIR}/CPwdGroup.cpp"
            "${KEYBOX_UTILITIES_DIR}/CAsymmetricKeyPair.cpp"
            "${KEYBOX_UTILITIES_DIR}/CRequest.cpp"
            "${KEYBOX_UTILITIES_DIR}/CipherEngine_linux.cpp"
            "${KEYBOX_UTILITIES_DIR}/InitGlobalRG_linux.cpp")

    target_include_directories(utilities PUBLIC
            "${KEYBOX_UTILITIES_DIR}"
            "${KEYBOX_ANDROID_DEPS_DIR}"
            "${KEYBOX_ANDROID_OPENSSL_ROOT}/include"
            "${KEYBOX_ANDROID_CURL_ROOT}/include")

    target_link_libraries(utilities PUBLIC
            "${OPENSSL_SSL_LIBRARY}"
            "${OPENSSL_CRYPTO_LIBRARY}"
            "${CURL_LIBRARY}"
            "${ZLIB_LIBRARY}")
else()
    add_subdirectory("${KEYBOX_UTILITIES_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/utilities_build")
endif()

add_library(keyboxa SHARED
        native-lib.cpp
)

target_include_directories(keyboxa PRIVATE "${KEYBOX_UTILITIES_DIR}")

target_link_options(keyboxa PRIVATE
        LINKER:-z,max-page-size=16384
)

target_link_libraries(keyboxa
        utilities
        log
)
