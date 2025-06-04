project "DropGame"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   staticruntime "off"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
  
   files 
   { 
      "src/**.h"
      , "src/**.cpp"
      , "src/**.hpp"
   }

   defines
	{
      "_CRT_SECURE_NO_WARNINGS"
      , "_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH"
      , "GLFW_INCLUDE_NONE"
	}

   includedirs
   {
      "%{wks.location}/Drop/dependencies/spdlog/include"
      , "%{wks.location}/Drop/src"
      , "%{wks.location}/Drop/dependencies"

      -- , "%{IncludeDir.Glad}"
      -- , "%{IncludeDir.GLFW}"
      -- , "%{IncludeDir.ImGui}"
      , "%{IncludeDir.glm}"
      -- , "%{IncludeDir.stb_image}"
      , "%{IncludeDir.assimp}"
      -- , "%{IncludeDir.yaml_cpp}"
   }

   links
   {
      "Drop"
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