cmake_minimum_required(VERSION 3.26)
project(keybox)

set(CMAKE_CXX_STANDARD 17)
enable_language(OBJCXX)

find_package(ZLIB REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})
set(HEADER_FILES
        ObjcKBFile.h
        ObjcPwdEntry.h
        ObjcMaskedBlob.h
        ObjcRandomGenerator.h
)
set(SOURCE_FILES
        ObjcKBFile.mm
        ObjcPwdEntry.mm
        ObjcMaskedBlob.mm
        ObjcRandomGenerator.mm
)

add_library(objc_utilities STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(objc_utilities PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(objc_utilities PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(objc_utilities PRIVATE "-framework Foundation")