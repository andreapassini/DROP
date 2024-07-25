project "DropGame"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "../dependencies/glad/include",
      "../dependencies/GLFW/include",
      "../dependencies/IMGUI",
      "../dependencies/glm/include",
      "../dependencies/stb_img/include",
      "../dependencies/assimp/includes",

      "../Drop/src",
        "../dependencies",

      -- "%{IncludeDir.glm}",
   }

   links
   {
      "Drop"
   }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
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