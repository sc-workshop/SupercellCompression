# astc


# astc options
if (MSVC)

set(ASTC_PREFIX "sse4.1")
set(ASTCENC_ISA_SSE41 ON)

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
