workspace "ScCompression"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }
	
	targetdir "%{wks.location}/build/bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    objdir "%{wks.location}/build/obj/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
	
	filter "configurations:Debug"
        runtime "Debug"

        defines {
            "DEBUG"
        }
        symbols "on"
        optimize "off"
    
    filter "configurations:Release"
        runtime "Release"

        defines {
            "NDEBUG"
        }
        symbols "off"
        optimize "Speed"
		
	filter ""
	
	group "Lib"
		include "core"
		include "./"
		include "./cli"
	
	group "Compression"
        include "dependencies/lzma"
        include "dependencies/lzham"
        include "dependencies/zstd"
		include "dependencies/astc"
		
	
	
	