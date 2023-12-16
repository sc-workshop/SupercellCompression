include(FetchContent)

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

add_library("SupercellCompression" STATIC ${Compression_Source} ${Compression_Headers})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Compression_Source} ${Compression_Headers})

# Core Setup
FetchContent_Declare(
    SupercellCore
    GIT_REPOSITORY https://github.com/sc-workshop/SC-Core.git
    GIT_TAG main
)
FetchContent_MakeAvailable(SupercellCore)

sc_core_base_setup("SupercellCompression")
set_target_properties("SupercellCompression" PROPERTIES
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
set(ISA_SSE41 ON)
set(CLI OFF)

FetchContent_Declare(
    astcenc
    GIT_REPOSITORY https://github.com/ARM-software/astc-encoder
    GIT_TAG 7e2a81ed5abc202c6f06be9302d193ba44a765c9 # 3.5
)
FetchContent_MakeAvailable(astcenc)
set_target_properties("astcenc-sse4.1-static" PROPERTIES
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

target_include_directories("SupercellCompression" PUBLIC
    "include/"
)

target_link_libraries("SupercellCompression" PUBLIC
    SupercellCore
)

target_include_directories(
    "SupercellCompression" PRIVATE

    ${astcenc_SOURCE_DIR}/Source
    ${lzham_codec_SOURCE_DIR}/include
    ${zstd_SOURCE_DIR}/lib
    ${lzma_sdk_SOURCE_DIR}/C
)

target_link_libraries(
    "SupercellCompression" PRIVATE

    libzstd_static
    astcenc-sse4.1-static
    lzhamlib
    LzmaLib
)
