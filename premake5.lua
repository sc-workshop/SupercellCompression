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
	}

    includedirs
	{
		"./include",
		"./core",
		"./dependencies/lzham/include",
		"./dependencies/lzma/include",
		"./dependencies/zstd/include",
		"./dependencies/astc/include"
	}
	
	links {
		"SupercellCore",
        "LZMA",
		"LZHAM",
		"Zstandard",
		"ASTC"
    }