project "Drop"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "off"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files 
   { 
      "src/**.h"
      , "src/**.cpp"
   
      , "dependencies/stb_image/**.h"
		, "dependencies/stb_image/**.cpp"
		, "dependencies/glm/glm/**.hpp"
		, "dependencies/glm/glm/**.inl"
   }

   defines
	{
		"GLM_ENABLE_EXPERIMENTAL"
      ,  "_CRT_SECURE_NO_WARNINGS"
      , "_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH"
	}


   includedirs
   {
      "src"
      
      , "dependencies/spdlog/include"

      , "%{IncludeDir.GLFW}"
		, "%{IncludeDir.Glad}"
		, "%{IncludeDir.ImGui}"
		, "%{IncludeDir.glm}"
		, "%{IncludeDir.stb_image}"
      , "%{IncludeDir.assimp}"
		, "%{IncludeDir.yaml_cpp}"
   }

   libdirs { 
      "dependencies/assimp/libs"
   }

   links
   {
      "Glad"
      
      , "GLFW"
      
      -- GUI
      , "ImGui"

      -- Yaml
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

      -- Windows specific
      , "shlwapi.lib"
   }


   filter "system:windows"
      systemversion "latest"
      defines { "DROP_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "DROP_DEBUG" }
      runtime "Debug"
      optimize "Off"
      symbols "On"
		-- buildoptions "/MDd"
      -- buildoptions "/MD"
      buildoptions "/MT"

   filter "configurations:Release"
      defines { "DROP_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"
      -- buildoptions "/MD"
      buildoptions "/MT"

   filter "configurations:Dist"
      defines { "DROP_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"
      -- buildoptions "/MD"
      buildoptions "/MT"