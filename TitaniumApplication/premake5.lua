project "TitaniumApplication"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/TitaniumRenderer/vendor/spdlog/include",
		"%{wks.location}/TitaniumRenderer/src",
		"%{wks.location}/TitaniumRenderer/vendor",
		"{IncludeDir.glm}"
	}

	links
	{
		"TitaniumRenderer"
	}

	filter "system:linux"
		links
		{
			"GLFW",
			"Glad",
			"ImGui",
			"Xrandr",
			"Xi",
			"GLU",
			"GL",
			"X11",
			"dl",
			"pthread",
			"stdc++fs"	--GCC versions 5.3 through 8.x need stdc++fs for std::filesystem
		}

	filter "system:windows"
		systemversion "latest"
	
		defines
		{
		}

    filter "configurations:Debug"
		defines "TD_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "TD_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "TD_DIST"
		runtime "Release"
		optimize "on"