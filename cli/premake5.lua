
project "SupercellCompressionCLI"
    kind "ConsoleApp"

    language "C++"
    cppdialect "C++17"

    files {
        "source/**"
    }

    includedirs {
		"%{wks.location}/include",
		"%{wks.location}/core",
		"%{wks.location}/dependencies/lzham/include",
		"%{wks.location}/dependencies/lzma/include",
		"%{wks.location}/dependencies/zstd/include",
		"%{wks.location}/dependencies/astc/include"
    }
	
	links {
        "SupercellCompression"
    }
