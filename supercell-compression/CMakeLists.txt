# supercell-compression


# C++ headers and source files
file(GLOB_RECURSE HEADERS source/*.h)
file(GLOB_RECURSE SOURCES source/*.cpp)

# choosing build type
if(SC_COMPRESSION_BUILD_SHARED)
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

# link supercell::core library
target_link_libraries(${TARGET} PUBLIC supercell::core)

# setup project
sc_project_setup(${TARGET})

# add compression dependencies
include(cmake/lzma.cmake)
include(cmake/lzham.cmake)
include(cmake/zstd.cmake)
include(cmake/astc.cmake)

target_include_directories(${TARGET} PRIVATE ${lzma-sdk_SOURCE_DIR}/C ${lzham_codec_SOURCE_DIR}/include ${zstd_SOURCE_DIR}/lib ${astcenc_SOURCE_DIR}/Source)

target_link_libraries(${TARGET} PRIVATE lzma-lib lzham-lib libzstd_static astcenc-${ASTC_PREFIX}-static)
