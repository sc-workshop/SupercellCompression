project "LZMA"
    kind "StaticLib"

    language "C"

    files {
		"src/**.c"
    }
	
	includedirs
	{
		"include"
	}

    filter "system: not windows"
        defines {
            "WIN32=0"
        }

