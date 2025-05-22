-- DropExternal.lua

-- IncludeDir = {}
-- IncludeDir["yaml_cpp"] = "../dependencies/yaml-cpp/include"

group "Dependencies"
   include "dependencies/glad"
   include "dependencies/GLFW"
   include "dependencies/imgui"
   include "dependencies/yaml-cpp"
group ""

group "Core"
include "Drop"
group ""