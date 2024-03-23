workspace "DROP"
    architecture "x64"

    configurations{
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}" -- BuildName/x64/Windows


--- include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glad"] = "dependencies/glad/include"
IncludeDir["GLFW"] = "dependencies/GLFW/include"
IncludeDir["IMGUI"] = "dependencies/IMGUI/include"

include "dependencies/glad"
include "dependencies/GLFW"
include "dependencies/IMGUI"

project "DROP"
    location "DROP"
    kind "ConsoleApp"
    language "C++"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    files{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs{
        -- "dependencies/glfw/include",
        "dependencies/glm/include",
        "dependencies/stb_img/include",
        "dependencies/assimp/includes",

        "%{IncludeDir.glad}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.IMGUI}",
    }

    libdirs { 
        -- "dependencies/glfw/lib-vc2022",
        "dependencies/assimp/libs"
    }


    links{  --- The name of the projects
        -- glad
        "glad",
    
        "GLFW",
        -- "glfw3.lib",
        -- "glfw3_mt.lib",
        -- "opengl32.lib",

        -- ASSIMP
        "assimp-vc143-mt.lib",
        "draco.lib",
        "kubazip.lib",
        "minizip.lib",
        "poly2tri.lib",
        "pugixml.lib",
        "zlib.lib",

        -- IMGUI
        "IMGUI",

        "gdi32.lib",
        "user32.lib",
        "Shell32.lib",
        "Advapi32.lib"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "10.0"

        defines{
            "DROP_PLATFORM_WINDOWS",
            "_UNICODE",
            "UNICODE"
        }

    filter "configurations:Debug"
        defines "DROP_DEBUG"
        symbols "On"
        buildoptions "/MT"

    filter "configurations:Release"
        defines "DROP_RELEASE"
        optimize "On"
        buildoptions "/MT"

    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"