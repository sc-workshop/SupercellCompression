# zstd

# zstandard options
if(NOT DEFINED ZSTD_BUILD_SHARED)
	set(ZSTD_BUILD_STATIC ON)
	set(ZSTD_BUILD_SHARED OFF)
endif()
set(ZSTD_LEGACY_SUPPORT OFF)
set(ZSTD_BUILD_DICTBUILDER OFF)
set(ZSTD_BUILD_DEPRECATED OFF)
set(ZSTD_BUILD_PROGRAMS OFF)
set(ZSTD_BUILD_TESTS OFF)
set(ZSTD_BUILD_CONTRIB OFF)

# download zstd module
FetchContent_Declare(
    zstd
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SOURCE_SUBDIR build/cmake
    URL "https://github.com/facebook/zstd/releases/download/v1.5.7/zstd-1.5.7.tar.gz"
	FIND_PACKAGE_ARGS
)

# populate zstd
FetchContent_MakeAvailable(zstd)

# move into dependencies folder
if (TARGET libzstd_static)
	set_target_properties(libzstd_static PROPERTIES
		FOLDER supercell-sdk/dependencies
	)

	set_target_properties(clean-all PROPERTIES
		FOLDER supercell-sdk/dependencies
	)

	set_target_properties(uninstall PROPERTIES
		FOLDER supercell-sdk/dependencies
	)

	add_library(zstd::libzstd ALIAS libzstd_static)
endif()
