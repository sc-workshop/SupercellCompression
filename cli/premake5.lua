
project "SupercellCompressionCLI"
    kind "ConsoleApp"

    language "C++"
    cppdialect "C++17"

    files {
        "main.cpp"
    }

    includedirs {
		"%{wks.location}/include"
    }
	
	links {
        "SupercellCompression"
    }

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"
        optimize "off"
    
    filter "configurations:Release"
        defines "NDEBUG"
        runtime "Release"
        symbols "off"
        optimize "on"
