project "SupercellCompression"
    kind "StaticLib"

    language "C++"
    cppdialect "C++17"
	
	-- Sources
	files {
		"source/**.cpp"
	}
	
	-- Headers
	files {
		"include/**.h",
		"source/**.h",
		"core/**.h"
	}

    includedirs {
        "include"
	}
	
	links {
        "LZMA",
		"LZHAM",
		"Zstandard"
    }