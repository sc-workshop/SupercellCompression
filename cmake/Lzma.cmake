# Custom CMake module for LZHAM
cmake_minimum_required(VERSION 3.26)

FetchContent_Declare(
    lzma_sdk
    GIT_REPOSITORY https://github.com/jljusten/LZMA-SDK
    GIT_TAG 781863cdf592da3e97420f50de5dac056ad352a5
)

if(NOT lzma_sdk_POPULATED)
    FetchContent_Populate(lzma_sdk)
endif()

set(LZMA_LIB_SOURCE_DIR ${lzma_sdk_SOURCE_DIR}/C)

set(LZMA_LIB_Sources
    "${LZMA_LIB_SOURCE_DIR}/Alloc.c"
    "${LZMA_LIB_SOURCE_DIR}/LzFind.c"
    "${LZMA_LIB_SOURCE_DIR}/LzmaDec.c"
    "${LZMA_LIB_SOURCE_DIR}/LzmaEnc.c"
    "${LZMA_LIB_SOURCE_DIR}/7zFile.c"
    "${LZMA_LIB_SOURCE_DIR}/7zStream.c"
)

add_library("LzmaLib" STATIC ${LZMA_LIB_Sources})

target_include_directories("LzmaLib" PRIVATE
    "${LZMA_LIB_SOURCE_DIR}"
)

target_compile_definitions("LzmaLib" PRIVATE
    _7ZIP_ST
)

target_compile_options("LzmaLib" PRIVATE
    $<$<AND:${SC_MSVC},${SC_RELEASE}>: /Ox /GF /Gy /GS- /Ob2 /Oi /Ot>
    $<$<AND:${SC_GNU},${SC_RELEASE}>: -c -O2 -Wall>
)