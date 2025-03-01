# astc

# astc options
if (MSVC)
    if ("${WK_PREFERRED_CPU_FEATURES}" STREQUAL "AVX2")
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
else()

    set(ASTC_PREFIX "native")
    set(ASTCENC_ISA_NATIVE ON)

endif()

set(ASTCENC_SHAREDLIB OFF)
set(ASTCENC_CLI OFF)

# download astc module
FetchContent_Declare(
    astcenc
    GIT_REPOSITORY https://github.com/ARM-software/astc-encoder
    GIT_TAG 4.7.0
)
FetchContent_MakeAvailable(astcenc)
