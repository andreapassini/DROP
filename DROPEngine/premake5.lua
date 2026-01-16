project "DropEngine"
   kind "SharedLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "off"

   -- targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	-- objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
   -- DLL in the same dir as executable, the simplest solution
   targetdir ("%{wks.location}/bin/" .. outputdir .. "/Drop")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/Drop")
   symbolspath ("%{wks.location}/bin/" .. outputdir .. "/Drop/$(ProjectName)-$([System.Guid]::NewGuid()).pdb")

   files 
   { 
      "src/**.h"
      , "src/**.cpp"
      , "src/**.hpp"
   }

   defines
   {
		"GLM_ENABLE_EXPERIMENTAL"
      , "_CRT_SECURE_NO_WARNINGS"
      , "_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH"
      , "GLFW_INCLUDE_NONE"
   }

   includedirs
   {
      "src"
      , "%{wks.location}/DropGame/src"
      , "%{IncludeDir.GLFW}"
      , "%{IncludeDir.glad}"
      , "%{IncludeDir.ImGui}"
      , "%{IncludeDir.glm}"
      , "%{IncludeDir.stb_img}"
      , "%{IncludeDir.assimp}"
      , "%{IncludeDir.spdlog}"
      , "%{IncludeDir.yaml_cpp}"
   }

   includedirs
   {
      -- "%{wks.location}/Drop/dependencies/spdlog/include"
      -- , "%{wks.location}/Drop/src"
      -- , "%{wks.location}/Drop/dependencies"

      "%{IncludeDir.glad}"
      , "%{IncludeDir.GLFW}"
      , "%{IncludeDir.ImGui}"
      , "%{IncludeDir.glm}"
      , "%{IncludeDir.stb_image}"
      , "%{IncludeDir.assimp}"
      , "%{IncludeDir.yaml_cpp}"
   }

   libdirs 
   { 
      "%{IncludeDir.assimp_lib}"
   }

   links
   {
      -- Windows specific
      "shlwapi.lib"

      -- Game specific
      -- , "DropGame"

      -- Libs
      , "GLFW"
      , "glad"
      , "ImGui"
      , "yaml-cpp"

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
   }


   filter "system:windows"
      systemversion "latest"
      defines { "DROP_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "DROP_DEBUG" }
      runtime "Debug"
      optimize "Off"
      symbols "On"
      buildoptions "/MTd /LD"

   filter "configurations:Release"
      defines { "DROP_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"
      buildoptions "/MT /LD"

   filter "configurations:Dist"
      defines { "DROP_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"
      buildoptions "/MT /LD"
