project "Zstandard"
    kind "StaticLib"

    language "C"

    files {"src/**.c"}

    includedirs {
		"include",
		"include/common",
		"include/compress",
		"include/decompress"
	}

    defines {"ZSTD_DISABLE_ASM", "ZSTD_MULTITHREAD"}

    filter {"system:windows"}
        defines {"__USE_MINGW_ANSI_STDIO"}

