project "yaml-cpp"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		
		"include/**.h"
	}

	includedirs
	{
		"include"
	}

	defines
	{
      "_CRT_SECURE_NO_WARNINGS"
      , "_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "off"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "off"

	filter "configurations:Debug"
		runtime "Debug"
      	optimize "Off"
      	symbols "On"
		-- buildoptions "/MDd"
      	-- buildoptions "/MD"
      	buildoptions "/MT"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
      	symbols "On"
      	-- buildoptions "/MD"
		buildoptions "/MT"

	filter "configurations:Dist"
      runtime "Release"
      optimize "On"
      symbols "Off"
    --   buildoptions "/MD"
      buildoptions "/MT"