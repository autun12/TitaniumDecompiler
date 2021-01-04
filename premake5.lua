workspace "TitaniumDecompiler"
	architecture "x86_64"
	startproject "TitaniumApplication"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/TitaniumRenderer/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/TitaniumRenderer/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/TitaniumRenderer/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/TitaniumRenderer/vendor/glm"
group "Dependencies"
	include "vendor/premake"
	include "TitaniumRenderer/vendor/GLFW"
	include "TitaniumRenderer/vendor/Glad"
	include "TitaniumRenderer/vendor/imgui"
group ""

include "TitaniumRenderer"
include "TitaniumDecompiler"
include "TitaniumApplication"