# astc

set(ASTC_VERSION 5.3.0)
message(STATUS "ASTC VERSION: ${ASTC_VERSION}")

set(ASTCENC_SHAREDLIB OFF)
set(ASTCENC_CLI OFF)
set(ASTC_PREFIX "")

# ASTC options
if(MSVC OR UNIX AND NOT APPLE)
    # Trying to setup CPU instrincts based on current env on Windows
    if("${WK_PREFERRED_CPU_FEATURES}" STREQUAL "AVX2")
        set(ASTC_PREFIX "avx2")
        set(ASTCENC_ISA_AVX2 ON)
    elseif("${WK_PREFERRED_CPU_FEATURES}" STREQUAL "SSE41")
        set(ASTC_PREFIX "sse4.1")
        set(ASTCENC_ISA_SSE41 ON)
    elseif("${WK_PREFERRED_CPU_FEATURES}" STREQUAL "SSE2")
        set(ASTC_PREFIX "sse2")
        set(ASTCENC_ISA_SSE2 ON)
    elseif("${WK_PREFERRED_CPU_FEATURES}" STREQUAL "Default")
        set(ASTC_PREFIX "native")
    endif()
elseif(APPLE)
    # On Apple platforms, we build a universal binary for both x86_64 and arm64
    add_custom_target(
        astcenc ALL
            COMMAND
                lipo -create -output $<TARGET_FILE_DIR:astcenc-sse4.1-static>/libastcenc-static.a -arch x86_64 $<TARGET_FILE:astcenc-sse4.1-static> -arch x86_64h $<TARGET_FILE:astcenc-avx2-static> -arch arm64 $<TARGET_FILE:astcenc-neon-static>
            VERBATIM
        
        DEPENDS
            astcenc-sse4.1-static
            astcenc-avx2-static
            astcenc-neon-static
    )
else()
    # Native build for other platforms
    set(ASTC_PREFIX "native")
    set(ASTCENC_ISA_NATIVE ON)
endif()

# download astc module
FetchContent_Declare(
    astcenc
    GIT_REPOSITORY https://github.com/ARM-software/astc-encoder
    GIT_TAG 5.3.0
)
FetchContent_MakeAvailable(astcenc)

if (NOT ${ASTC_PREFIX} STREQUAL "")
    add_library(astc::astcenc ALIAS astcenc-${ASTC_PREFIX}-static)
elseif(APPLE)
    add_library(astc::astcenc STATIC IMPORTED GLOBAL)

    set_target_properties(astc::astcenc PROPERTIES
        IMPORTED_LOCATION "${astcenc_BINARY_DIR}/Source/$<CONFIG>/libastcenc-static.a"
        IMPORTED_LOCATION_DEBUG
            "${astcenc_BINARY_DIR}/Source/Debug/libastcenc-static.a"
        IMPORTED_LOCATION_RELEASE
            "${astcenc_BINARY_DIR}/Source/Release/libastcenc-static.a"
        IMPORTED_LOCATION_RELWITHDEBINFO
            "${astcenc_BINARY_DIR}/Source/RelWithDebInfo/libastcenc-static.a"
        IMPORTED_LOCATION_MINSIZEREL
            "${astcenc_BINARY_DIR}/Source/MinSizeRel/libastcenc-static.a"
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    )

    add_dependencies(astc::astcenc astcenc)
else()
    add_library(astc::astcenc ALIAS astcenc-native-static)
endif()
