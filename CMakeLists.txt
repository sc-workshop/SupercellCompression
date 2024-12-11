cmake_minimum_required(VERSION 3.22)

# workspace
project(supercell-sdk)

# options
option(SC_COMPRESSION_BUILD_CLI "Build CLI version of supercell::compression library" OFF)

# includes
include(FetchContent)

if(SC_COMPRESSION_BUILD_CLI)
    set(WK_CLI_DEPENDENCIES ON)
endif()

# SC metadata
set(WK_SERIALIZATION_FLATBUFFER ON)

include(cmake/dependencies.cmake)

# projects
add_subdirectory(supercell-compression)

if(SC_COMPRESSION_BUILD_CLI)
    add_subdirectory(cli-tool)
endif()
