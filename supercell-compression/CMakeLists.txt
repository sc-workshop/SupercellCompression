# Project: supercell-compression

# TODO(pavidloq): make public and private sources
# C++ headers and source files
file(GLOB_RECURSE HEADERS source/*.h)
file(GLOB_RECURSE SOURCES source/*.cpp)

# choosing build type
if(WK_BUILD_SHARED)
    # shared library
    set(TARGET supercell-compression-shared)

    add_library(${TARGET} SHARED)
    add_library(supercell::compression ALIAS ${TARGET})
else()
    # static library
    set(TARGET supercell-compression-static)

    add_library(${TARGET} STATIC)
    add_library(supercell::compression ALIAS ${TARGET})
endif()

# add *.h and *.cpp files
target_sources(${TARGET} PRIVATE ${HEADERS} ${SOURCES})

# include directories
target_include_directories(${TARGET} PUBLIC source)

# link wk::core library
target_link_libraries(
    ${TARGET} PUBLIC 
    wk::core flatbuffers
)

set_target_properties(${TARGET} PROPERTIES
    FOLDER supercell-sdk/core
)

# setup project
wk_project_setup(${TARGET})

# add compression dependencies
include(cmake/lzma.cmake)
include(cmake/lzham.cmake)
include(cmake/zstd.cmake)
include(cmake/astc.cmake)

target_include_directories(${TARGET} PRIVATE ${lzma-sdk_SOURCE_DIR}/C ${lzham_codec_SOURCE_DIR}/include ${zstd_SOURCE_DIR}/lib ${astcenc_SOURCE_DIR}/Source)

target_link_libraries(${TARGET} PRIVATE lzma-lib lzham-lib zstd::libzstd astcenc-${ASTC_PREFIX}-static)

wk_include_flatbuffers()