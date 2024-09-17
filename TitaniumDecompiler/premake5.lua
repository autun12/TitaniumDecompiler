project "TitaniumDecompiler"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "tdpch.h"
	pchsource "src/tdpch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"src",
		"${IncludeDir.Capstone}"
	}

	links
	{
		"Capstone"
	}
	
	filter "system:linux"
		links
		{
			"Capstone",
			"stdc++fs"	--GCC versions 5.3 through 8.x need stdc++fs for std::filesystem
		}

		defines
		{
			"TD_PLATFORM_LINUX"
		}
    
	filter "system:windows"
		systemversion "latest"

		links
		{
		}

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