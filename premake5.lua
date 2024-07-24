-- premake5.lua
workspace "DropGame"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "DropGame"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "DropExternal.lua"
include "DropGame"