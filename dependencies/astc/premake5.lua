
project "ASTC"
    kind "StaticLib"

    language "C++"
    cppdialect "C++14"

    files {
		"include/**.h",
		"source/**.cpp",
		"source/**.h"
    }

    includedirs {
        "source",
		"include"
    }
	
	filter {"system:linux or macos"}
		links {"pthread"}
		
	filter {"toolset:msc"}
		buildoptions { 
			"/wd4324", 
			"/EHsc",
			"/fp:precise", "/fp:contract"
		}
		
		
	filter {"toolset:gcc or clang"}
		buildoptions 
		{ 
			"-Wall",
			"-Wextra",
			"-Wpedantic",
			"-Werror",
			"-Wshadow",
			"-Wdouble-promotion",
			
			"-Wno-unknown-warning-option",
			"-Wno-c++98-compat-pedantic",
			"-Wno-c++98-c++11-compat-pedantic",
			"-Wno-float-equal",
			"-Wno-deprecated-declarations",
			"-Wno-atomic-implicit-seq-cst",
			"-Wno-cast-align",
			
			"-ffp-contract=fast"
		}
		
	filter {"toolset:clang"}
		buildoptions 
		{ 
			"-Wdocumentation",
			"-gdwarf-4",
			"-Wno-nonportable-system-include-path"
		}
		
	-- Define AVX for PC
	
	filter {"system:windows or linux", "architecture:x86_64"}
		defines
		{
			"ASTCENC_NEON=0",
			"ASTCENC_SSE=41",
			"ASTCENC_AVX=2",
			"ASTCENC_POPCNT=2",
			"ASTCENC_F16C"
		}
	
	filter {"system:windows", "architecture:x86_64"}
		buildoptions 
		{
			"/arch:AVX2"
		}
	
	filter {"system:windows or linux",  "architecture:x86_64", "toolset:gcc or clang"}
		buildoptions
		{
			"-mavx2 -mpopcnt -mf16c",
			"-Wno-unused-command-line-argument"
		}
	
