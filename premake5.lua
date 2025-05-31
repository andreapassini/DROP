-- premake5.lua
include "DROPExternal.lua"

workspace "Drop"
   architecture "x86_64"
   startproject "DropGame"
   configurations { "Debug", "Release", "Dist" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
   include "Drop/dependencies/GLFW"
	include "Drop/dependencies/Glad"
   include "Drop/dependencies/ImGui"
   include "Drop/dependencies/yaml-cpp"
group ""

group "Core"
   include "Drop"
group ""

group "Tools"
   include "DropGame"
group ""