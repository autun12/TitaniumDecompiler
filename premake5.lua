workspace "TitaniumDecompiler"
	architecture "x86_64"
	startproject "TitaniumDecompiler"

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
IncludeDir["GLFW"] = "%{wks.location}/TitaniumDecompiler/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/TitaniumDecompiler/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/TitaniumDecompiler/vendor/imgui"

group "Dependencies"
	include "vendor/premake"
	include "TitaniumDecompiler/vendor/GLFW"
	include "TitaniumDecompiler/vendor/Glad"
	include "TitaniumDecompiler/vendor/imgui"
group ""

include "TitaniumDecompiler"