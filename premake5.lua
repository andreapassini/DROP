-- premake5.lua
workspace "Drop"
   architecture "x86_64"
   startproject "Drop"
   configurations { "Debug", "Release", "Dist" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "DROPExternal.lua"

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