include(FetchContent)

set(TARGET "SupercellCompression")

set(Compression_Headers
    "include/SupercellCompression.h"

    "include/SupercellCompression/interface/CompressionInterface.h"
    "include/SupercellCompression/interface/DecompressionInterface.h"
    "include/SupercellCompression/interface/ImageCompressionInterface.h"
    "include/SupercellCompression/interface/ImageDecompressionInterface.h"

    "include/SupercellCompression/exception/Astc.h"
    "include/SupercellCompression/exception/Lzham.h"
    "include/SupercellCompression/exception/Lzma.h"
    "include/SupercellCompression/exception/Zstd.h"

    "include/SupercellCompression/Astc.h"
    "include/SupercellCompression/KhronosTexture.h"
    "include/SupercellCompression/Lzham.h"
    "include/SupercellCompression/Lzma.h"
    "include/SupercellCompression/ScCompression.h"
    "include/SupercellCompression/Zstd.h"

    "include/SupercellCompression/Astc/Compressor.h"
    "include/SupercellCompression/Astc/Decompressor.h"

    "include/SupercellCompression/Lzham/Compressor.h"
    "include/SupercellCompression/Lzham/Decompressor.h"

    "include/SupercellCompression/Lzma/Compressor.h"
    "include/SupercellCompression/Lzma/Decompressor.h"

    "include/SupercellCompression/Zstd/Compressor.h"
    "include/SupercellCompression/Zstd/Decompressor.h"
)

set(Compression_Source
    "source/Astc/Astc.cpp"
    "source/Astc/Compressor.cpp"
    "source/Astc/Decompressor.cpp"

    "source/Lzham/Compressor.cpp"
    "source/Lzham/Decompressor.cpp"

    "source/Lzma/Compressor.cpp"
    "source/Lzma/Decompressor.cpp"

    "source/Sc/Compressor.cpp"
    "source/Sc/Decompressor.cpp"
    "source/Zstd/Compressor.cpp"
    "source/Zstd/Decompressor.cpp"

    "source/Image/KhronosTexture.cpp"
)

add_library(${TARGET} STATIC ${Compression_Source} ${Compression_Headers})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Compression_Source} ${Compression_Headers})

# Core Setup
FetchContent_Declare(
    SupercellCore
    GIT_REPOSITORY https://github.com/sc-workshop/SC-Core.git
    GIT_TAG main
)
FetchContent_MakeAvailable(SupercellCore)

#include("${SupercellCore_SOURCE_DIR}/cmake/constants.cmake")
sc_core_base_setup(${TARGET})
set_target_properties(${TARGET} PROPERTIES
    FOLDER Supercell
)

# Compression Dependecies
message("-- ZStandart --")
set(ZSTD_BUILD_STATIC ON)
set(ZSTD_BUILD_SHARED OFF)
set(ZSTD_LEGACY_SUPPORT OFF)
set(ZSTD_BUILD_DICTBUILDER OFF)
set(ZSTD_BUILD_DEPRECATED OFF)
set(ZSTD_BUILD_PROGRAMS OFF)

FetchContent_Declare(
    zstd
    URL "https://github.com/facebook/zstd/releases/download/v1.5.5/zstd-1.5.5.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SOURCE_SUBDIR build/cmake
)
FetchContent_MakeAvailable(zstd)
set_target_properties("libzstd_static" PROPERTIES
    FOLDER Compression
)

message("-- ASTC Encoder --")

if (MSVC)

set(ASTC_PREFIX "sse4.1")
set(ASTCENC_ISA_SSE41 ON)

else()

set(ASTC_PREFIX "native")
set(ASTCENC_ISA_NATIVE ON)

endif()

set(ASTCENC_SHAREDLIB OFF)
set(ASTCENC_CLI OFF)

FetchContent_Declare(
    astcenc
    GIT_REPOSITORY https://github.com/ARM-software/astc-encoder
    GIT_TAG 1a51f2915121275038677317c8bf61f1a78b590c # 4.7.0
)
FetchContent_MakeAvailable(astcenc)
set_target_properties("astcenc-${ASTC_PREFIX}-static" PROPERTIES
    FOLDER Compression
)

include(cmake/Lzma.cmake)
set_target_properties("LzmaLib" PROPERTIES
    FOLDER Compression
)

include(cmake/Lzham.cmake)
set_target_properties("lzhamlib" PROPERTIES
    FOLDER Compression
)

target_include_directories(${TARGET} PUBLIC
    "include/"
)

target_link_libraries(${TARGET} PUBLIC
    SupercellCore
)

target_include_directories(
    ${TARGET} PRIVATE

    ${astcenc_SOURCE_DIR}/Source
    ${lzham_codec_SOURCE_DIR}/include
    ${zstd_SOURCE_DIR}/lib
    ${lzma_sdk_SOURCE_DIR}/C
)

target_link_libraries(
    ${TARGET} PRIVATE

    libzstd_static
    astcenc-${ASTC_PREFIX}-static
    lzhamlib
    LzmaLib
)