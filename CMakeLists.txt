cmake_minimum_required(VERSION 3.22)

# workspace
project(supercell-sdk)

# options
option(SC_COMPRESSION_BUILD_CLI "Build CLI version of supercell::compression library" OFF)

# includes
include(FetchContent)

include(cmake/dependencies.cmake)
include(WkFlatbuffers)
include(WkArgsparse)

# projects
add_subdirectory(supercell-compression)

if(SC_COMPRESSION_BUILD_CLI)
    include(WkJson)
    add_subdirectory(cli-tool)
endif()
 