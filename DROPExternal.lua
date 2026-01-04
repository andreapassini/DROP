-- DropExternal.lua

IncludeDir = {}
-- IncludeDir["stb_image"] = "%{wks.location}/Drop/dependencies/stb_image/include"
-- IncludeDir["yaml_cpp"] = "%{wks.location}/Drop/dependencies/yaml-cpp/include"
IncludeDir["GLFW"] = "%{wks.location}/Drop/dependencies/GLFW/include"
IncludeDir["glad"] = "%{wks.location}/Drop/dependencies/glad/include"
IncludeDir["spdlog"] = "%{wks.location}/Drop/dependencies/spdlog/include"
-- IncludeDir["ImGui"] = "%{wks.location}/Drop/dependencies/ImGui"
-- IncludeDir["glm"] = "%{wks.location}/Drop/dependencies/glm"
-- IncludeDir["assimp"] = "%{wks.location}/Drop/dependencies/assimp/includes"

group "Dependencies"
   include "Drop/dependencies/GLFW"
   include "Drop/dependencies/glad"
   -- include "Drop/dependencies/ImGui"
   -- include "Drop/dependencies/yaml-cpp"
group ""

group "Core"
   include "Drop"
group ""

group "Tools"
   include "DropGame"
group ""