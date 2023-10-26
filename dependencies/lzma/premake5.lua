project "LZMA"
    kind "StaticLib"

    language "C"

    files {
		"src/**.c"
    }

    filter "system: not windows"
        defines {
            "WIN32=0"
        }

