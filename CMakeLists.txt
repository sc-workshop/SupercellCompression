﻿cmake_minimum_required(VERSION 3.22)

option(SC_COMPRESSION_CLI "Build CLI for Supercell Compression" OFF)

project("SupercellCompression")
include(cmake/SupercellCompression.cmake)

if(${SC_COMPRESSION_CLI})
    include(cmake/CLI.cmake)
endif()