project "Drop"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "src",
      
      "../dependencies/glad/include",
      "../dependencies/GLFW/include",
      "../dependencies/imgui",
      
      "../dependencies/glm/include",
      "../dependencies/stb_img/include",
      "../dependencies/assimp/includes",
      "../dependencies/spdlog/include",
   }

   libdirs { 
      "../dependencies/assimp/libs"
   }

   links
   {
      "glad"
      
      , "GLFW"
      
      , "ImGui"

      -- ASSIMP
      , "assimp-vc143-mt.lib"
      , "draco.lib"
      , "kubazip.lib"
      , "minizip.lib"
      , "poly2tri.lib"
      , "pugixml.lib"
      , "zlib.lib"

      , "gdi32.lib"
      , "user32.lib"
      , "Shell32.lib"
      , "Advapi32.lib"

      , "shlwapi.lib"
   }

   targetdir ("bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "DROP_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "DROP_DEBUG" }
      runtime "Debug"
      symbols "On"
      buildoptions "/MT"

   filter "configurations:Release"
      defines { "DROP_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"
      buildoptions "/MT"

   filter "configurations:Dist"
      defines { "DROP_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"
      buildoptions "/MT"