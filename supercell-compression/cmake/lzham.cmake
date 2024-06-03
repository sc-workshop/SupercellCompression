# lzham


# custom CMake module for LZHAM
cmake_minimum_required(VERSION 3.22)

# includes
include(FetchContent)

# install lzham
FetchContent_Declare(
    lzham_codec
    GIT_REPOSITORY https://github.com/richgel999/lzham_codec.git
    GIT_TAG d379b1f9121e2197881c61cfc4713c78848bdfe7
)

# populate lzham
if(NOT lzham_codec_POPULATED)
    FetchContent_Populate(lzham_codec)
endif()

# lzham sources
set(LZHAM_DEC_SOURCE_DIR ${lzham_codec_SOURCE_DIR}/lzhamdecomp)
set(LZHAM_CMP_SOURCE_DIR ${lzham_codec_SOURCE_DIR}/lzhamcomp)

set(LZHAM_Compressor_Sources
    ${LZHAM_CMP_SOURCE_DIR}/lzham_lzbase.cpp
    ${LZHAM_CMP_SOURCE_DIR}/lzham_lzbase.h
    ${LZHAM_CMP_SOURCE_DIR}/lzham_lzcomp.cpp
    ${LZHAM_CMP_SOURCE_DIR}/lzham_lzcomp_internal.cpp
    ${LZHAM_CMP_SOURCE_DIR}/lzham_lzcomp_internal.h
    ${LZHAM_CMP_SOURCE_DIR}/lzham_lzcomp_state.cpp
    ${LZHAM_CMP_SOURCE_DIR}/lzham_match_accel.cpp
    ${LZHAM_CMP_SOURCE_DIR}/lzham_match_accel.h
    ${LZHAM_CMP_SOURCE_DIR}/lzham_null_threading.h
    ${LZHAM_CMP_SOURCE_DIR}/lzham_pthreads_threading.cpp
    ${LZHAM_CMP_SOURCE_DIR}/lzham_threading.h

    $<${SC_MSVC}: ${LZHAM_CMP_SOURCE_DIR}/lzham_win32_threading.cpp>
)

set(LZHAM_Decompressor_Sources
    ${LZHAM_DEC_SOURCE_DIR}/lzham_assert.cpp
    ${LZHAM_DEC_SOURCE_DIR}/lzham_assert.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_checksum.cpp
    ${LZHAM_DEC_SOURCE_DIR}/lzham_checksum.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_config.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_core.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_decomp.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_helpers.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_huffman_codes.cpp
    ${LZHAM_DEC_SOURCE_DIR}/lzham_huffman_codes.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_lzdecompbase.cpp
    ${LZHAM_DEC_SOURCE_DIR}/lzham_lzdecompbase.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_lzdecomp.cpp
    ${LZHAM_DEC_SOURCE_DIR}/lzham_math.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_mem.cpp
    ${LZHAM_DEC_SOURCE_DIR}/lzham_mem.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_platform.cpp
    ${LZHAM_DEC_SOURCE_DIR}/lzham_platform.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_prefix_coding.cpp
    ${LZHAM_DEC_SOURCE_DIR}/lzham_prefix_coding.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_symbol_codec.cpp
    ${LZHAM_DEC_SOURCE_DIR}/lzham_symbol_codec.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_timer.cpp
    ${LZHAM_DEC_SOURCE_DIR}/lzham_timer.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_traits.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_types.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_utils.h
    ${LZHAM_DEC_SOURCE_DIR}/lzham_vector.cpp
    ${LZHAM_DEC_SOURCE_DIR}/lzham_vector.h
)

# static library
set(LZHAM_TARGET lzham-lib)
add_library(${LZHAM_TARGET} STATIC
    ${LZHAM_Compressor_Sources}
    ${LZHAM_Decompressor_Sources}
    ${lzham_codec_SOURCE_DIR}/lzhamlib/lzham_lib.cpp
)

# include directories
target_include_directories(${LZHAM_TARGET} PRIVATE
    "${lzham_codec_SOURCE_DIR}/include"
    "${LZHAM_DEC_SOURCE_DIR}"
    "${LZHAM_CMP_SOURCE_DIR}"
)

# preprocessor definitions
target_compile_definitions(${LZHAM_TARGET} PRIVATE
    $<${SC_RELEASE}:NDEBUG>
    $<${SC_DEBUG}:_DEBUG>
)

# flags for non-x64 systems
if($<NOT:${SC_X64}>)
    target_compile_options(${LZHAM_TARGET} PRIVATE
        $<${SC_GNU}: -m32>
    )
endif()

# compile options
target_compile_options(${LZHAM_TARGET} PRIVATE
    $<${SC_GNU}: -Wall -Wextra -fno-strict-aliasing -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64>
    $<$<AND:${SC_GNU},${SC_DEBUG}>: -g>
    $<$<AND:${SC_GNU},${SC_RELEASE}>: -O3 -fomit-frame-pointer -fexpensive-optimizations>

    $<$<AND:${SC_MSVC},${SC_DEBUG}>: /RTC1>
    $<$<AND:${SC_MSVC},${SC_RELEASE}>: /GS- /Gy /fp:fast /W4 /Ox /Ob2 /Oi /Ot /Oy>
)

# TODO(Daniil-SV): Make .patch for LZHAM to make it work on MacOS
