project "Drop"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   staticruntime "off"

   -- targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	-- objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
   -- DLL in the same dir as executable, the simplest solution
   targetdir ("%{wks.location}/bin/" .. outputdir .. "/Drop")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/Drop")
   -- symbolspath '$(ProjectName)-$([System.Guid]::NewGuid()).pdb'

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
      , "%{wks.location}/DropEngine/src"
   }

   libdirs 
   { 
   }

   links
   {
      -- Windows specific
      "shlwapi.lib"

      -- Engine specific
      -- , "DropEngine" -- Do it your selfs for live coding
   }


   filter "system:windows"
      systemversion "latest"
      defines { "DROP_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "DROP_DEBUG" }
      runtime "Debug"
      optimize "Off"
      symbols "On"
      -- buildoptions {"/MT", "/nologo", "/Gm", "/GR", "/EH", "/Od", "/Oi", "/WX", "/W4", "/wd4201", "/wd4100", "/wd4189", "/LD"}
      buildoptions {"/MT", "/nologo", "/Gm", "/GR", "/EH", "/Od", "/Oi", "/W4", "/wd4201", "/wd4100", "/wd4189", "/LD"}

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