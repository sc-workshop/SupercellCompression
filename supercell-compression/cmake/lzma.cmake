# lzma


# custom CMake module for LZMA
cmake_minimum_required(VERSION 3.22)

# includes
include(FetchContent)

# install lzma-sdk
FetchContent_Declare(
    lzma-sdk
    GIT_REPOSITORY https://github.com/jljusten/LZMA-SDK
    GIT_TAG 781863cdf592da3e97420f50de5dac056ad352a5
)
FetchContent_MakeAvailable(lzma-sdk)

# setup lzma-sdk sources
set(LZMA_SDK_SOURCE_DIR ${lzma-sdk_SOURCE_DIR}/C)

set(LZMA_SDK_SOURCES
    ${LZMA_SDK_SOURCE_DIR}/Alloc.c
    ${LZMA_SDK_SOURCE_DIR}/LzFind.c
    ${LZMA_SDK_SOURCE_DIR}/LzmaDec.c
    ${LZMA_SDK_SOURCE_DIR}/LzmaEnc.c
    ${LZMA_SDK_SOURCE_DIR}/7zFile.c
    ${LZMA_SDK_SOURCE_DIR}/7zStream.c
)

# static library
set(LZMA_TARGET lzma-lib)
add_library(${LZMA_TARGET} STATIC ${LZMA_SDK_SOURCES})

target_include_directories(${LZMA_TARGET} PRIVATE ${LZMA_SDK_SOURCE_DIR})

target_compile_definitions(${LZMA_TARGET} PRIVATE
    _7ZIP_ST
)

target_compile_options(${LZMA_TARGET} PRIVATE
    $<$<AND:${SC_MSVC},${SC_RELEASE}>: /Ox /GF /Gy /GS- /Ob2 /Oi /Ot>
    $<$<AND:${SC_GNU},${SC_RELEASE}>: -c -O2 -Wall>
)
