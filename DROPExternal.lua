-- DropExternal.lua

-- IncludeDir = {}
-- IncludeDir["glm"] = "../vendor/glm"

group "Dependencies"
   include "dependencies/glad"
   include "dependencies/GLFW"
   include "dependencies/imgui"
group ""

group "Core"
include "Drop"
group ""