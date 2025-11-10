-- premake5.lua
include "DROPExternal.lua"

workspace "Drop"
   architecture "x86_64"
   startproject "Drop"
   configurations { "Debug", "Release", "Dist" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
   -- include "Drop/dependencies/GLFW"
   -- include "Drop/dependencies/Glad"
   -- include "Drop/dependencies/ImGui"
   -- include "Drop/dependencies/yaml-cpp"
group ""

group "Core"
   include "Drop"
group ""

group "Tools"
   include "DropGame"
group ""

newaction {
    trigger = "clean",
    description = "Remove all binaries and intermediate binaries, and vs files.",
    execute = function()
        print("Removing binaries")
        os.rmdir("./bin")
        print("Removing intermediate binaries")
        os.rmdir("./bin-int")
        print("Removing .pdb")
        os.remove("**.pdb")
        print("Removing project files")
        os.rmdir("./.vs")
        os.remove("**.sln")
        os.remove("**.vcxproj")
        os.remove("**.vcxproj.filters")
        os.remove("**.vcxproj.user")
        print("Done")
    end
}

newaction {
    trigger = "cleanPdb",
    description = "Remove all .pdb.",
    execute = function()
        print("Removing .pdb")
        os.remove("**.pdb")
        os.remove("**.idb")
        print("Done")
    end
}