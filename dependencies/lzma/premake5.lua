
project "LZMA"
    kind "StaticLib"

    language "C"

    targetdir "%{wks.location}/build/bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    objdir "%{wks.location}/build/obj/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"

    files {
		"src/**.c"
    }

    includedirs {
        "include"
	}

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
        optimize "on"

    filter "system: not windows"
        defines {
            "WIN32=0"
        }

