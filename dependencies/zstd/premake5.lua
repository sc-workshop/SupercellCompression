project "Zstandard"
    kind "StaticLib"

    language "C"

    targetdir "%{wks.location}/build/bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"
    objdir "%{wks.location}/build/obj/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}/%{prj.name}"

    files {"src/**.c"}

    includedirs {"include", "include/common", "include/compress", "include/decompress"}

    defines {"ZSTD_DISABLE_ASM", "ZSTD_MULTITHREAD"}

    filter {"system:windows"}
        defines {"__USE_MINGW_ANSI_STDIO"}

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

