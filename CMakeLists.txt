cmake_minimum_required(VERSION 3.22)


# workspace
project(supercell-sdk)

# options
option(SC_COMPRESSION_IN_SOURCE_BUILD "Enable in-source build of supercell::compression" ON)

option(SC_COMPRESSION_BUILD_SHARED "Build supercell::compression library as shared library" OFF)
option(SC_COMPRESSION_BUILD_CLI "Build CLI version of supercell::compression library" ON)


# lets force C++17
set(CMAKE_CXX_STANDARD 17)

# includes
include(FetchContent)

include(cmake/dependencies.cmake)


# projects
add_subdirectory(supercell-compression)

if(SC_COMPRESSION_BUILD_CLI)
    add_subdirectory(cli-tool)
endif()
